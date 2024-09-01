#include "stdafx.h"

#include "GameServer.h"
#include "GroupDefine.h"
#include "LobbyGroup.h"
#include "GameGroup.h"
#include "Player.h"
#include "Common/Token.h"
#include "LoginGroup.h"
#include "GameHost.h"
#include "common/AppSettings.h"

void game::GameServer::OnStart()
{
}

void game::GameServer::OnStop()
{
}

bool game::GameServer::OnConnectionRequest(const NetworkAddress& netInfo)
{
	return true;
}

void game::GameServer::OnAccept(const NetworkAddress& netInfo, uint64 sessionId)
{
	Player* player = AddHost<Player>(sessionId);
	SetGroup(sessionId, (uint8)Groups::LOGIN);
}

void game::GameServer::OnDisconnect(uint64 sessionId)
{
	Player* player = GetHost<Player>(sessionId);

	if (player != nullptr)
	{
		RemoveHost(player);
	}
	else
	{
		CRASH();
	}
}

game::GameServer::~GameServer()
{
}


game::GameServer::GameServer(const json& setting)
	:HeteroServerBase(
		NetworkAddress(INADDR_ANY, setting["port"].get<uint16>()),
		setting["worker_thread"].get<int32>(),
		setting["concurrent_thread"].get<int32>(),
		setting["session_limit"].get<int32>()
	)
{
	EnablePacketEncoding(setting["packet_code"].get<BYTE>(), setting["packet_key"].get<BYTE>());
	_redis = new RedisSession();
	bool ret = _redis->connect(AppSettings::GetSection("Redis")["host"],
		AppSettings::GetSection("Redis")["port"].get<uint16>(),
		"", 3000
	);


	_groups[(uint8)Groups::LOGIN] = CreateGroup<LoginGroup>((uint8)Groups::LOGIN, 20);
	_groups[(uint8)Groups::LOGIN]->SetServer(this);
	_groups[(uint8)Groups::LOBBY] = CreateGroup<LobbyGroup>((uint8)Groups::LOBBY, 20);
	_groups[(uint8)Groups::LOBBY]->SetServer(this);
	_groups[(uint8)Groups::CANYON] = CreateGroup<GameGroup>((uint8)Groups::CANYON, 20);
	_groups[(uint8)Groups::CANYON]->SetServer(this);
	_groups[(uint8)Groups::CEMETERY] = CreateGroup<GameGroup>((uint8)Groups::CEMETERY, 20);
	_groups[(uint8)Groups::CEMETERY]->SetServer(this);
}

void game::GameServer::RemoveHost(GameHost* ptr)
{
	{
		WRITE_LOCK(_lock);
		_hosts.erase(ptr->_sessionId);
	}

	freeFunc(ptr);

}

bool game::GameServer::GetToken(uint64 accountId, Token& token)
{
	bool result = _redis->get(to_string(accountId), token.token);

	return result;
}
