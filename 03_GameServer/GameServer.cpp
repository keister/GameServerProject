#include "stdafx.h"

#include "GameServer.h"
#include "GroupDefine.h"
#include "LobbyGroup.h"
#include "GameGroup_Town.h"
#include "Player.h"
#include "Common/Token.h"
#include "LoginGroup.h"
#include "GameHost.h"

void GameServer::OnStart()
{
}

void GameServer::OnStop()
{
}

bool GameServer::OnConnectionRequest(const NetworkAddress& netInfo)
{
	return true;
}

void GameServer::OnAccept(const NetworkAddress& netInfo, uint64 sessionId)
{
	Player* player = AddHost<Player>(sessionId);
	SetGroup(sessionId, (uint8)Groups::LOGIN);
}

void GameServer::OnDisconnect(uint64 sessionId)
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

GameServer::~GameServer()
{
}


GameServer::GameServer(const wstring& name)
	:HeteroServerBase(name)
{
	_redis = new RedisSession();
	bool ret = _redis->connect("procademyserver.iptime.org", 11772, "", 3000);


	_groups[(uint8)Groups::LOGIN] = CreateGroup<LoginGroup>((uint8)Groups::LOGIN, 20);
	_groups[(uint8)Groups::LOGIN]->SetServer(this);
	_groups[(uint8)Groups::LOBBY] = CreateGroup<LobbyGroup>((uint8)Groups::LOBBY, 20);
	_groups[(uint8)Groups::LOBBY]->SetServer(this);
	_groups[(uint8)Groups::TOWN] = CreateGroup<GameGroup_Town>((uint8)Groups::TOWN, 20);
	_groups[(uint8)Groups::TOWN]->SetServer(this);
}

void GameServer::RemoveHost(GameHost* ptr)
{
	{
		WRITE_LOCK(_lock);
		_hosts.erase(ptr->_sessionId);
	}

	freeFunc(ptr);

}

bool GameServer::GetToken(uint64 accountId, Token& token)
{
	bool result = _redis->get(to_string(accountId), token.token);

	return result;
}
