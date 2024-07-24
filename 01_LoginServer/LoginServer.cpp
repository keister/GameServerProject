#include "stdafx.h"
#include "LoginServer.h"
#include "Packet.h"
#include "Thread.h"
#include "ProcessMonitor.h"
#include "NetworkAddress.h"
#include "Player.h"
#include "PacketHandler.h"
#include "Common/PacketDefine.h"

LoginServer::LoginServer(const wstring& name)
	: ServerBase(name)
	, _dbConnectionTlsIndex(TlsAlloc())
	, _redisConnectionTlsIndex(TlsAlloc())
{


	_gameServerIps = { L"127.0.0.1" };
	_gameServerPorts = { 11757 };
	_chatServerIps = { L"127.0.0.1" };
	_chatServerPorts = { 11755 };

	//_monitoringClient = new MonitoringClient({ L"127.0.0.1", 11704 }, 1, 1, false, 109, 30);
	//_monitoringClient->Connect();
	//_monitorThread = RunThread(&LoginServer::func_monitor_thread, this);

	_redis = new RedisSession();
	_redis->connect("127.0.0.1", 11772);
}

LoginServer::~LoginServer()
{
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
	delete_user(sessionId);

}

void LoginServer::OnRecv(uint64 sessionId, Packet& pkt)
{
	Player* player = find_user(sessionId);

	HandlePacket_LoginServer(this, *player, pkt);

}

void LoginServer::Handle_C_REQ_LOGIN(Player& player, int64 accountNo, Token& token)
{
	SQLSession& session = get_sql_session();
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
		//return fail
	}

	Token gameToken;
	Token::Generate(&gameToken);

	mysqlx::Schema sch = session.getSchema("login");

	uint64 id;

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


	if (set_token(id, gameToken) == false)
	{
		LOG_DBG(L"LoginServer", L"Token insert Fail (%d)", accountNo);

		//Packet& sendPkt = make_s_res_login(accountNo, 0, TODO, TODO, TODO, TODO);
		//SendPacket(user.sessionId, sendPkt);
		return;
	}

	SendPacket(player.sessionId, Make_S_RES_LOGIN(id, true, gameToken));
	InterlockedIncrement64((LONGLONG*)&_authCount);
}

void LoginServer::Handle_C_GET_SERVER_LIST(Player& player)
{

	SendPacket(player.sessionId, Make_S_GET_SERVER_LIST(_gameServerIps, _gameServerPorts));
}

Player* LoginServer::find_user(uint64 sessionId)
{
	int32 mapIdx = sessionId % NUM_OF_USER_MAP;

	Player* user = nullptr;

	{
		READ_LOCK(_mapLock[mapIdx]);

		auto it = _userMap[mapIdx].find(sessionId);
		if (it == _userMap[mapIdx].end())
		{
			return nullptr;
		}
		user = it->second;
	}

	return user;

}

void LoginServer::delete_user(uint64 sessionId)
{
	int32 mapIdx = sessionId % NUM_OF_USER_MAP;

	Player* user = find_user(sessionId);
	if (user == nullptr)
	{
		return;
	}

	{
		WRITE_LOCK(_mapLock[mapIdx]);
		_userMap[mapIdx].erase(user->sessionId);
	}

	_userPool.Free(user);


}

void LoginServer::insert_user(uint64 sessionId, const NetworkAddress& netInfo)
{
	Player* user = _userPool.Alloc();
	user->sessionId = sessionId;
	user->ip = netInfo.GetIpAddress();
	user->lastRecvTime = GetTickCount64();

	int32 mapIdx = sessionId % NUM_OF_USER_MAP;
	WRITE_LOCK(_mapLock[mapIdx]);
	_userMap[mapIdx].insert({ sessionId, user });

}



// Packet& LoginServer::make_SS_MONITOR_DATA_UPDATE(BYTE dataType, int32 dataValue, int32 timeStamp)
// {
// 	Packet& pkt = Packet::Alloc();
//
// 	pkt << (uint16)PacketType::SS_MONITOR_DATA_UPDATE << dataType << dataValue << timeStamp;
//
// 	return pkt;
// }

SQLSession& LoginServer::get_sql_session()
{
	SQLSession* session = static_cast<SQLSession*>(TlsGetValue(_dbConnectionTlsIndex));

	if (session == nullptr)
	{
		session = new SQLSession("localhost", 11771, "root", "as1234", "login");
		TlsSetValue(_dbConnectionTlsIndex, session);
	}

	return *session;
}

RedisSession& LoginServer::get_redis_session()
{
	RedisSession* session = static_cast<RedisSession*>(TlsGetValue(_redisConnectionTlsIndex));

	if (session == nullptr)
	{
		session = new RedisSession;
		{
			WRITE_LOCK(_lockRedis);
			session->connect("127.0.0.1", 11772);
		}
		TlsSetValue(_redisConnectionTlsIndex, session);
	}
	// if (session == nullptr)
	// {
	// 	session = new TokenStorage(L"127.0.0.1", 12530);
	// 	TlsSetValue(_redisConnectionTlsIndex, session);
	// }
	return *session;
}

bool LoginServer::set_token(uint64 accountNo, const Token& token)
{
	WRITE_LOCK(_lockRedis);
	uint32 retVal;

	//memcpy_s(s.data(), 64, token.token.data(), 64);
	return _redis->set(to_string(accountNo), token.token, retVal);
}

void LoginServer::func_timeout_thread()
{
	DWORD idealTime = timeGetTime();
	DWORD overTime = 0;
	while (true)
	{
		uint64 now = GetTickCount64();

		list<uint64> disconnectList;

		for (int32 idx = 0; idx < NUM_OF_USER_MAP; idx++)
		{
			for (auto it : _userMap[idx])
			{

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


			// 순회하면서 Disconnect까지 해버리면 OnDisconnect에서 Lock을 또 잡아서 DeadLock이 발생할 수 있다.
			// 따라서 DisconnectList에 넣어놓고 나중에 Disconnect를 호출한다.
			for (uint64 sessionId : disconnectList)
			{
				Disconnect(sessionId);
			}

			::Sleep(TIMEOUT_MS - overTime);
			idealTime += TIMEOUT_MS;
			overTime = timeGetTime() - idealTime;
			if (overTime >= TIMEOUT_MS)
			{
				overTime = TIMEOUT_MS;
			}

		}
	}
}
//
// void LoginServer::func_monitor_thread()
// {
// 	DWORD idealTime = timeGetTime();
// 	DWORD overTime = 0;
// 	uint64 prevAuthCount = 0;
// 	ProcessMonitor monitor;
// 	while (true)
// 	{
// 		monitor.Update();
// 		time_t timeStamp;
// 		time(&timeStamp);
// 		uint64 authCount = GetAuthCount();
// 		_monitoringClient->SendPacket(make_SS_MONITOR_DATA_UPDATE(MONITOR_DATA_TYPE_LOGIN_SERVER_RUN, 1, timeStamp));
//
// 		_monitoringClient->SendPacket(
// 			make_SS_MONITOR_DATA_UPDATE(MONITOR_DATA_TYPE_LOGIN_SERVER_CPU, monitor.GetTotalCpuUsage(), timeStamp)
// 		);
//
// 		_monitoringClient->SendPacket(
// 			make_SS_MONITOR_DATA_UPDATE(MONITOR_DATA_TYPE_LOGIN_SERVER_MEM, monitor.GetPrivateMemoryUsage() / 1000000, timeStamp)
// 		);
//
// 		_monitoringClient->SendPacket(
// 			make_SS_MONITOR_DATA_UPDATE(MONITOR_DATA_TYPE_LOGIN_SESSION, GetSessionCount(), timeStamp)
// 		);
//
// 		_monitoringClient->SendPacket(
// 			make_SS_MONITOR_DATA_UPDATE(MONITOR_DATA_TYPE_LOGIN_AUTH_TPS, authCount - prevAuthCount, timeStamp)
// 		);
//
// 		_monitoringClient->SendPacket(
// 			make_SS_MONITOR_DATA_UPDATE(MONITOR_DATA_TYPE_LOGIN_PACKET_POOL, Packet::GetUseCount(), timeStamp)
// 		);
//
// 		prevAuthCount = authCount;
//
// 		::Sleep(1000 - overTime);
// 		idealTime += 1000;
// 		overTime = timeGetTime() - idealTime;
// 		if (overTime >= 1000)
// 		{
// 			overTime = 1000;
// 		}
// 	}
// }
