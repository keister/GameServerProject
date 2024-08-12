#include "stdafx.h"
#include "ChatServer.h"

#include <mysqlx/xdevapi.h>

#include "PacketHandler.h"
#include "Job.h"
#include "SqlSession.h"
#include "Common/Token.h"

ChatServer::ChatServer(const wstring& name)
	: ServerBase(name)
	, _dbRead(2)
{
	_redis = new RedisSession();
	bool ret = _redis->connect("procademyserver.iptime.org", 11772, "", 3000);
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

	InsertPlayer(*player);
}

void ChatServer::OnDisconnect(uint64 sessionId)
{
	Player* player = FindPlayer(sessionId);

	if (player == nullptr)
	{
		CRASH_LOG(L"ChattingServer_Multi", L"Invalid User SessionId(%llu)", sessionId);
	}
	else
	{

		DeletePlayer(*player);

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

Player* ChatServer::FindPlayer(uint64 sessionId)
{
	READ_LOCK(_playerMaplock);

	auto findIt = _players.find(sessionId);

	if (findIt == _players.end())
	{
		return nullptr;
	}

	return findIt->second;
}

void ChatServer::DeletePlayer(Player& player)
{
	WRITE_LOCK(_playerMaplock);
	_players.erase(player.SessionId());
}

void ChatServer::InsertPlayer(Player& player)
{
	WRITE_LOCK(_playerMaplock);
	_players.insert({ player.SessionId(), &player });
}

ChatServer::~ChatServer()
{
}

bool ChatServer::GetToken(uint64 accountId, Token& token)
{
	WRITE_LOCK(_redisLock);
	bool result = _redis->get(to_string(accountId), token.token);

	return result;
}

void ChatServer::OnRecv(uint64 sessionId, Packet pkt)
{
	Player* player = FindPlayer(sessionId);
	if (player == nullptr)
	{
		CRASH();
	}

	WRITE_LOCK(player->lock);

	if (sessionId != player->SessionId())
	{
		LOG_ERR(L"ChattServer", L"Not Match Session Id (origin : %d, wrong : %d)", sessionId, player->SessionId());
		return;
	}
	HandlePacket_ChatServer(this, *player, pkt);
}

void ChatServer::Handle_C_CHAT_LOGIN(Player& player, uint64 accountId, Token& token)
{
	Token tk;
	GetToken(accountId, tk);

	if (tk != token)
	{
		SendPacket(player.SessionId(), Make_S_CHAT_LOGIN(false));
		return;
	}
	uint64 sessionId = player.SessionId();
	_dbRead.ExecuteAsync(Job::Alloc(
		[this, &player, accountId, sessionId]
		{
			WRITE_LOCK(player.lock);
			if (sessionId != player.SessionId())
			{
				LOG_ERR(L"ChattServer", L"Not Match Session Id (origin : %d, wrong : %d)", sessionId, player.SessionId());
				return;
			}

			SqlSession& sql = GetSqlSession();

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
	));
}

void ChatServer::Handle_C_CHAT_ENTER(Player& player, uint64 characterId, int32 fieldId)
{
	uint64 sessionId = player.SessionId();

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

				player.SetCharacter(characterId, row[0].get<wstring>(), fieldId);

				{
					WRITE_LOCK(_fieldLock[player.FieldId()]);
					_playersOnField[player.FieldId()].insert(&player);
				}

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

void ChatServer::Handle_C_MOVE_FIELD(Player& player, int32 fieldId)
{
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
