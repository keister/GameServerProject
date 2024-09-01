#include "stdafx.h"
#include "LoginServer.h"
#include "Packet.h"
#include "Thread.h"
#include "ProcessMonitor.h"
#include "NetworkAddress.h"
#include "Player.h"
#include "PacketHandler.h"
#include "SqlSession.h"
#include "common/AppSettings.h"
#include "Common/PacketDefine.h"



namespace login
{
	LoginServer::~LoginServer()
	{
	}

	LoginServer::LoginServer(const json& setting)
		: ServerBase(
			NetworkAddress(INADDR_ANY, setting["port"].get<uint16>()),
			setting["worker_thread"].get<int32>(),
			setting["concurrent_thread"].get<int32>(),
			setting["session_limit"].get<int32>()
		)
	{

		EnablePacketEncoding(setting["packet_code"].get<BYTE>(), setting["packet_key"].get<BYTE>());
		_redis = new RedisSession();
		_redis->connect(
			AppSettings::GetSection("Redis")["host"].get<string>(),
			AppSettings::GetSection("Redis")["port"].get<uint16>(),
			"",
			3000
		);

		_gameServerPorts = { 11757 };

		RunThread(&LoginServer::func_timeout_thread, this);
	}

	void LoginServer::OnStart()
	{
	}

	void LoginServer::OnStop()
	{
	}

	bool LoginServer::OnConnectionRequest(const NetworkAddress& netInfo)
	{
		return true;
	}

	void LoginServer::OnAccept(const NetworkAddress& netInfo, uint64 sessionId)
	{
		insert_user(sessionId, netInfo);
	}

	void LoginServer::OnDisconnect(uint64 sessionId)
	{
		delete_player(sessionId);

	}

	void LoginServer::OnRecv(uint64 sessionId, Packet pkt)
	{
		Player* player = find_player(sessionId);
		player->lastRecvTime = GetTickCount64();

		HandlePacket_LoginServer(this, *player, pkt);

	}

	void LoginServer::Handle_C_REQ_LOGIN(Player& player, int64 accountNo, Token& token)
	{

		// 플랫폼에 인증정보 검증 요청
		httplib::Client cli("https://procademyserver.iptime.org:11731");
		cli.enable_server_certificate_verification(false);

		json reqBody;
		reqBody["token"] = token.token;

		auto res = cli.Post("/api/auth/verify-token", reqBody.dump(), "application/json");
		auto resBody = json::parse(res->body);


		if (res->status != 200)
		{
			wstring st(res->body.begin(), res->body.end());
			LOG_ERR(L"http", L"%s", st);
		}

		uint64 accountId = resBody["id"].get<uint64>();
		string username = resBody["userName"].get<string>();


		if (accountNo != accountId)
		{
			SendPacket(player.sessionId, Make_S_RES_LOGIN(accountNo, false, {}));
			return;
		}

		// 무작위 문자열 토큰 생성
		Token gameToken;
		Token::Generate(&gameToken);

		SqlSession& session = GetSqlSession();
		mysqlx::Schema sch = session.getSchema("login");

		uint64 id;
		try
		{
			mysqlx::RowResult queryResult = sch.getTable("account")
				.select("id")
				.where("account_id = :accid")
				.bind("accid", accountId)
				.execute();


			if (queryResult.count() == 0)
			{
				auto insert = sch.getTable("account").insert("account_id", "username").values(accountId, username).execute();
				id = insert.getAutoIncrementValue();
			}
			else
			{
				id = queryResult.fetchOne()[0].get<uint64>();
			}
		}
		catch (const mysqlx::Error& err)
		{
			const char* errStr = err.what();
			wstring errString(errStr, errStr + strlen(errStr));
			CRASH_LOG(L"DB", L"DB Fail : %s", errString.c_str());

		}

		
		if (set_token(id, gameToken) == false) // 토큰 불일치
		{
			LOG_DBG(L"LoginServer", L"Token insert Fail (%d)", accountNo);

			SendPacket(player.sessionId, Make_S_RES_LOGIN(accountNo, false, {}));
			return;
		}

		player.isLogin = true;

		SendPacket(player.sessionId, Make_S_RES_LOGIN(id, true, gameToken));
		InterlockedIncrement64((LONGLONG*)&_authCount);
	}

	void LoginServer::Handle_C_GET_SERVER_LIST(Player& player)
	{
		SendPacket(player.sessionId, Make_S_GET_SERVER_LIST(_gameServerPorts));
	}

	Player* LoginServer::find_player(uint64 sessionId)
	{
		int32 mapIdx = sessionId % NUM_OF_USER_MAP;

		Player* user = nullptr;

		{
			READ_LOCK(_mapLock[mapIdx]);

			auto it = _playerMap[mapIdx].find(sessionId);
			if (it == _playerMap[mapIdx].end())
			{
				return nullptr;
			}
			user = it->second;
		}

		return user;

	}

	void LoginServer::delete_player(uint64 sessionId)
	{
		int32 mapIdx = sessionId % NUM_OF_USER_MAP;

		Player* user = find_player(sessionId);
		if (user == nullptr)
		{
			return;
		}

		{
			WRITE_LOCK(_mapLock[mapIdx]);
			_playerMap[mapIdx].erase(user->sessionId);
		}

		_playerAllocator.Free(user);
	}

	void LoginServer::insert_player(uint64 sessionId)
	{
		Player* user = _playerAllocator.Alloc();
		user->sessionId = sessionId;
		user->isLogin = false;
		user->lastRecvTime = GetTickCount64();

		// 세션아이디를 기반으로 map 분산
		int32 mapIdx = sessionId % NUM_OF_USER_MAP;
		WRITE_LOCK(_mapLock[mapIdx]);
		_playerMap[mapIdx].insert({ sessionId, user });

	}


	bool LoginServer::set_token(uint64 accountNo, const Token& token)
	{
		WRITE_LOCK(_lockRedis);
		uint32 retVal;

		//memcpy_s(s.data(), 64, token.token.data(), 64);
		return _redis->set(to_string(accountNo), token.token, retVal);
	}



	/// @brief 1초에 한번씩 유저들을 순회하며 커넥트만 하고 아무런 행동을 하지않는 유저들을 끊어낸다.
	void LoginServer::func_timeout_thread()
	{
		while (true)
		{
			uint64 now = GetTickCount64();

			list<uint64> disconnectList;

			for (int32 idx = 0; idx < NUM_OF_USER_MAP; idx++)
			{
				READ_LOCK(_mapLock[idx]);
				for (auto it : _playerMap[idx])
				{
					if (it.second->isLogin == true)
					{
						continue;
					}

					uint64 playerRecvTime = it.second->lastRecvTime;
					if (playerRecvTime > now)
					{
						continue;
					}

					uint64 timeDiff = now - playerRecvTime;

					if (timeDiff > TIMEOUT_MS)
					{
						disconnectList.push_back(it.second->sessionId);
						LOG_DBG(L"ChatServer_Multi", L"TimeOut - sessionId(%llu), diff(%llu), now(%llu), last(%llu)",
							it.second->sessionId, timeDiff, now, it.second->lastRecvTime);
					}
				}
			}

			// 순회하면서 Disconnect까지 해버리면 OnDisconnect에서 Lock을 또 잡아서 DeadLock이 발생할 수 있다.
			// 따라서 DisconnectList에 넣어놓고 나중에 Disconnect를 호출한다.
			for (uint64 sessionId : disconnectList)
			{
				Disconnect(sessionId);
			}

			::Sleep(1000);
		}
	}
}

