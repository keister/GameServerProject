#include "stdafx.h"
#include "GameServer.h"
#include "GameGroupBase.h"
#include "Player.h"


void GameGroupBase::SetServer(GameServer* server)
{
	_server = server;
}

Player* GameGroupBase::find_player(uint64 sessionId)
{
	auto findIt = _players.find(sessionId);

	if (findIt == _players.end())
	{
		return nullptr;
	}

	return findIt->second;
}

void GameGroupBase::insert_player(Player* player)
{
	_players.insert({ player->sessionId, player });
}

void GameGroupBase::delete_player(uint64 sessionId)
{
	_players.erase(sessionId);
}
