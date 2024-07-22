#include "stdafx.h"

#include "GameServer.h"
#include "GroupDefine.h"
#include "LobbyGroup.h"
#include "GameGroup_Town.h"
#include "Player.h"
#include "Common/Token.h"
#include "LoginGroup.h"

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
	Player* player = _playerPool.Alloc(sessionId);
	InsertPlayer(player);

	SetGroup(sessionId, (uint8)Groups::LOGIN);
}

void GameServer::OnDisconnect(uint64 sessionId)
{
	Player* player = FindPlayer(sessionId);

	if (player != nullptr)
	{

		DeletePlayer(player);
		_playerPool.Free(player);
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
	_redis->connect("127.0.0.1", 6379);

	_groups[(uint8)Groups::LOGIN] = CreateGroup<LoginGroup>((uint8)Groups::LOGIN);
	_groups[(uint8)Groups::LOGIN]->SetServer(this);
	_groups[(uint8)Groups::LOBBY] = CreateGroup<LobbyGroup>((uint8)Groups::LOBBY);
	_groups[(uint8)Groups::LOBBY]->SetServer(this);
	_groups[(uint8)Groups::TOWN] = CreateGroup<GameGroup_Town>((uint8)Groups::TOWN, 20);
	_groups[(uint8)Groups::TOWN]->SetServer(this);
}

Player* GameServer::FindPlayer(uint64 sessionId)
{
	READ_LOCK(_lock);
	auto findIt = _players.find(sessionId);

	if (findIt == _players.end())
	{
		return nullptr;
	}

	return findIt->second;
}

void GameServer::InsertPlayer(Player* player)
{
	WRITE_LOCK(_lock);
	_players.insert({ player->sessionId, player });
}

void GameServer::DeletePlayer(Player* player)
{
	WRITE_LOCK(_lock);
	_players.erase(player->sessionId);
}

bool GameServer::GetToken(uint64 accountId, Token& token)
{
	bool result = _redis->get(to_string(accountId), token.token);

	return result;
}
