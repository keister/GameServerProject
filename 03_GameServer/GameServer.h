#pragma once
#include "GameGroupBase.h"
#include "GroupDefine.h"
#include "HeteroServerBase.h"

#include "../Libs/hiredis/CRedisConn.h"
class GameHost;
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

	template <typename T> requires is_base_of_v<GameHost, T>
	T* GetHost(uint64 sessionId)
	{
		READ_LOCK(_lock);

		auto findIt = _hosts.find(sessionId);

		if (findIt == _hosts.end())
		{
			return nullptr;
		}

		return (T*)findIt->second;
	}

	template <typename T> 
	T* AddHost(uint64 sessionId)
	{
		T* ret = GlobalPool<T>::Alloc();

		{
			WRITE_LOCK(_lock);
			if (freeFunc == nullptr)
			{
				freeFunc = GlobalPool<T>::Free;
			}
			ret->_sessionId = sessionId;
			_hosts.insert({sessionId, ret});
		}

		return ret;
	}

	void RemoveHost(GameHost* ptr);

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
	RedisSession* _redis;
	unordered_map<uint64, GameHost*> _hosts;
	Lock _lock;

	GameGroupBase* _groups[(int32)Groups::NUM_GROUPS];

	using FreeFuncType = void(*)(void*);

	FreeFuncType freeFunc = nullptr;
};

