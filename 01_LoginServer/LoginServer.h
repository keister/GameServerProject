#pragma once
#include "../Libs/hiredis/CRedisConn.h"
#include "mysqlx/xdevapi.h"
#include "ObjectPoolTls.h"
#include "ServerBase.h"
#include "Common/Token.h"
class MonitoringClient;
using SQLSession = mysqlx::Session;
using RedisSession = RedisCpp::CRedisConn;

class Player;

class LoginServer : public ServerBase
{
	enum
	{
		NUM_OF_USER_MAP = 128,
		TIMEOUT_MS = 50000,
	};

public:
	LoginServer(const wstring& name);
	~LoginServer() override;

	uint64 GetAuthCount() { return _authCount; }

protected:
	void OnStart() override;
	void OnStop() override;
	bool OnConnectionRequest(const NetworkAddress& netInfo) override;
	void OnAccept(const NetworkAddress& netInfo, uint64 sessionId) override;
	void OnDisconnect(uint64 sessionId) override;
	void OnRecv(uint64 sessionId, Packet& pkt) override;

	//@@@AutoPackBegin
	friend bool HandlePacket_LoginServer(LoginServer*, Player&, Packet&);
	void Handle_C_REQ_LOGIN(Player& player, int64 accountNo, Token& token);
	void Handle_C_GET_SERVER_LIST(Player& player);

	//@@@AutoPackEnd

private:
	Player* find_user(uint64 sessionId);
	void delete_user(uint64 sessionId);
	void insert_user(uint64 sessionId, const NetworkAddress& netinfo);

	//static Packet& make_SS_MONITOR_DATA_UPDATE(BYTE dataType, int32 dataValue, int32 timeStamp);


	SQLSession& get_sql_session();
	RedisSession& get_redis_session();

	bool set_token(uint64 accountNo, const Token& token);

	void func_timeout_thread();
	void func_monitor_thread();
private:
	unordered_map<uint64, Player*> _userMap[NUM_OF_USER_MAP];
	Lock						 _mapLock[NUM_OF_USER_MAP];

	uint64						_authCount;

	vector<wstring>				_gameServerIps;
	vector<uint16>				_gameServerPorts;
	vector<wstring>				_chatServerIps;
	vector<uint16>				_chatServerPorts;

	DWORD						 _dbConnectionTlsIndex;
	DWORD						 _redisConnectionTlsIndex;
	RedisSession*				_redis;
	Lock						 _lockRedis;
	ObjectPoolTls<Player, false>	 _userPool;
	MonitoringClient*			_monitoringClient;

	HANDLE _monitorThread;
};

