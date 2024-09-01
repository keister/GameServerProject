#include "stdafx.h"
#include "ChatServer.h"

#include <mysqlx/xdevapi.h>

#include "PacketHandler.h"
#include "Job.h"
#include "SqlSession.h"
#include "common/AppSettings.h"
#include "Common/Token.h"

namespace chat
{
	ChatServer::ChatServer(const json& setting)
		: ServerBase(
			NetworkAddress(INADDR_ANY, setting["port"].get<uint16>()),
			setting["worker_thread"].get<int32>(),
			setting["concurrent_thread"].get<int32>(),
			setting["session_limit"].get<int32>()
		)
		, _dbRead(2)
	{
		EnablePacketEncoding(setting["packet_code"].get<BYTE>(), setting["packet_key"].get<BYTE>());
		_redis = new RedisSession();
		bool ret = _redis->connect(
			AppSettings::GetSection("Redis")["host"].get<string>(),
			AppSettings::GetSection("Redis")["port"].get<uint16>(),
			"",
			3000
		);
	}

	void ChatServer::OnStart()
	{
	}

	void ChatServer::OnStop()
	{
	}

	bool ChatServer::OnConnectionRequest(const NetworkAddress& netInfo)
	{
		return false;
	}

	void ChatServer::OnAccept(const NetworkAddress& netInfo, uint64 sessionId)
	{
		Player* player = _playerPool.Alloc();

		{
			WRITE_LOCK(player->lock);
			player->Init(sessionId);
		}

		insert_player(*player);
	}

	void ChatServer::OnDisconnect(uint64 sessionId)
	{
		Player* player = find_player(sessionId);

		if (player == nullptr)
		{
			CRASH_LOG(L"ChattingServer_Multi", L"Invalid User SessionId(%llu)", sessionId);
		}
		else
		{

			delete_player(*player);

			WRITE_LOCK(player->lock);

			{
				WRITE_LOCK(_nicknameToPlayerLock);
				_nicknameToPlayer.erase(player->Nickname());
			}

			int32 fieldId = player->FieldId();
			if (fieldId != -1)
			{
				WRITE_LOCK(_fieldLock[fieldId]);
				_playersOnField[fieldId].erase(player);
			}


			_playerPool.Free(player);
		}
	}

	Player* ChatServer::find_player(uint64 sessionId)
	{
		READ_LOCK(_playerMaplock);

		auto findIt = _players.find(sessionId);

		if (findIt == _players.end())
		{
			return nullptr;
		}

		return findIt->second;
	}

	void ChatServer::delete_player(Player& player)
	{
		WRITE_LOCK(_playerMaplock);
		_players.erase(player.SessionId());
	}

	void ChatServer::insert_player(Player& player)
	{
		WRITE_LOCK(_playerMaplock);
		_players.insert({ player.SessionId(), &player });
	}

	ChatServer::~ChatServer()
	{
	}


	/// @brief Redis 서버에서 accountId로 로그인서버에서 등록한 토큰정보를 가져온다.
	/// @param accountId	로그인서버 account ID
	/// @param token		토큰을 담을 공간
	/// @return 성공 / 실패
	bool ChatServer::get_token(uint64 accountId, Token& token)
	{
		WRITE_LOCK(_redisLock);
		bool result = _redis->get(to_string(accountId), token.token);

		return result;
	}

	void ChatServer::OnRecv(uint64 sessionId, Packet pkt)
	{
		Player* player = find_player(sessionId);
		if (player == nullptr)
		{
			CRASH();
		}

		WRITE_LOCK(player->lock);

		// 플레이어를 재사용하기 때문에, 이전 플레이어의 패킷이 올 수 있음
		if (sessionId != player->SessionId())
		{
			LOG_ERR(L"ChatServer", L"Not Match Session Id (origin : %d, wrong : %d)", sessionId, player->SessionId());
			return;
		}
		HandlePacket_ChatServer(this, *player, pkt);
	}

	void ChatServer::Handle_C_CHAT_LOGIN(Player& player, uint64 accountId, Token& token)
	{
		Token tk;
		get_token(accountId, tk);

		if (tk != token)
		{
			SendPacket(player.SessionId(), Make_S_CHAT_LOGIN(false));
			return;
		}

		// 읽기작업 비동기 요청
		_dbRead.ExecuteAsync(Job::Alloc(
			[this, &player, accountId, sessionId = player.SessionId()]
			{
				WRITE_LOCK(player.lock);
				if (sessionId != player.SessionId())
				{
					LOG_ERR(L"ChattServer", L"Not Match Session Id (origin : %d, wrong : %d)", sessionId, player.SessionId());
					return;
				}

				SqlSession& sql = GetSqlSession();

				try
				{
					mysqlx::Schema sch = sql.getSchema("game");
					mysqlx::RowResult result = sch.getTable("player")
						.select("id")
						.where("account_id = :ID")
						.bind("ID", accountId)
						.execute();

					mysqlx::Row row = result.fetchOne();

					player.SetPlayerId(row[0].get<uint64>());
					SendPacket(player.SessionId(), Make_S_CHAT_LOGIN(true));
				}
				catch (const mysqlx::Error& err)
				{
					const char* errStr = err.what();
					wstring errString(errStr, errStr + strlen(errStr));
					CRASH_LOG(L"DB", L"DB Fail : %s", errString.c_str());

				}
			}
		)
		);
	}

	void ChatServer::Handle_C_CHAT_ENTER(Player& player, uint64 characterId)
	{
		uint64 sessionId = player.SessionId();


		// 읽기작업 비동기 요청
		_dbRead.ExecuteAsync(Job::Alloc(
			[=, this, &player]
			{
				WRITE_LOCK(player.lock);
				if (sessionId != player.SessionId())
				{
					LOG_ERR(L"ChattServer", L"Not Match Session Id (origin : %d, wrong : %d)", sessionId, player.SessionId());
					return;
				}

				SqlSession& sql = GetSqlSession();
				try
				{
					mysqlx::Schema sch = sql.getSchema("game");
					mysqlx::RowResult result = sch.getTable("character")
						.select("nickname")
						.where("id = :ID")
						.bind("ID", characterId)
						.execute();

					mysqlx::Row row = result.fetchOne();

					player.SetCharacter(characterId, row[0].get<wstring>());

					{
						WRITE_LOCK(_nicknameToPlayerLock);
						_nicknameToPlayer.insert({ player.Nickname(), &player });
					}


					SendPacket(player.SessionId(), Make_S_CHAT_ENTER(true));
				}
				catch (mysqlx::Error& err)
				{
					const char* errStr = err.what();
					wstring errString(errStr, errStr + strlen(errStr));
					CRASH_LOG(L"DB", L"DB Fail : %s", errString.c_str());
				}
			}
		));
	}

	void ChatServer::Handle_C_CHAT_LEAVE(Player& player)
	{
	}

	void ChatServer::Handle_C_CHAT_MOVE_FIELD(Player& player, int32 fieldId)
	{
		if (player.FieldId() == fieldId)
		{
			return;
		}

		{
			if (player.FieldId() != -1)
			{
				WRITE_LOCK(_fieldLock[player.FieldId()]);
				_playersOnField[player.FieldId()].erase(&player);
			}
		}

		{
			WRITE_LOCK(_fieldLock[fieldId]);
			_playersOnField[fieldId].insert(&player);
			player.SetField(fieldId);
		}
	}

	void ChatServer::Handle_C_CHAT(Player& player, uint8 chatType, wstring& message)
	{
		if (chatType == FIELD)
		{
			int32 fieldId = player.FieldId();
			if (fieldId == -1)
			{
				return;
			}

			Packet pkt = Make_S_CHAT(chatType, player.PlayerId(), player.CharacterId(), player.Nickname(), message);

			{
				READ_LOCK(_fieldLock[fieldId]);
				for (Player* p : _playersOnField[fieldId])
				{
					if (p == &player)
					{
						continue;
					}

					SendPacket(p->SessionId(), pkt);
				}
			}
		}
	}

	void ChatServer::Handle_C_WHISPER(Player& player, wstring& toNickname, wstring& message)
	{

		{
			WRITE_LOCK(_nicknameToPlayerLock);
			auto it = _nicknameToPlayer.find(toNickname);

			if (it == _nicknameToPlayer.end())
			{
				SendPacket(player.SessionId(), Make_S_CHAT_FAIL(NO_WHISPER_TARGET));
				return;
			}

			Player* target = it->second;
			SendPacket(target->SessionId(), Make_S_WHISPER(player.PlayerId(), player.CharacterId(), player.Nickname(), message));
		}

	}

}


