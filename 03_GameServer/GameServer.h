#pragma once
#include "GameGroupBase.h"
#include "GroupDefine.h"
#include "HeteroServerBase.h"

#include "../Libs/hiredis/CRedisConn.h"
class GameGroupBase;
class GameGroup_Town;
class LoginGroup;
class LobbyGroup;
using RedisSession = RedisCpp::CRedisConn;

class Player;
struct Token;

class GameServer : public HeteroServerBase
{

public:
	GameServer(const wstring& name);
	Player* FindPlayer(uint64 sessionId);
	void InsertPlayer(Player* player);
	void DeletePlayer(Player* player);

	bool GetToken(uint64 accountId, Token& token);

	uint64 GetPlayerCount(Groups group)
	{
		return _groups[(int32)group]->GetPlayerCount();
	}

	uint64 GetFrameCount(Groups group)
	{
		return _groups[(int32)group]->GetFrameCount();
	}

	float32 Get()
	{
		return _groups[0]->DeltaTime();
	}

protected:
	void OnStart() override;
	void OnStop() override;
	bool OnConnectionRequest(const NetworkAddress& netInfo) override;
	void OnAccept(const NetworkAddress& netInfo, uint64 sessionId) override;
	void OnDisconnect(uint64 sessionId) override;



public:
	~GameServer() override;


private:
	ObjectPoolTls<Player> _playerPool;
	RedisSession* _redis;
	unordered_map<uint64, Player*> _players;
	Lock _lock;

	GameGroupBase* _groups[(int32)Groups::NUM_GROUPS];
};

