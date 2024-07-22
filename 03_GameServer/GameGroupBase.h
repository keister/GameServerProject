#pragma once
#include "GroupBase.h"
class GameServer;
class Player;

class GameGroupBase : public GroupBase
{
public:
	GameGroupBase() {};

	void SetServer(GameServer* server);
	uint64 GetPlayerCount() { return _players.size(); }
protected:
	Player* find_player(uint64 sessionId);
	void insert_player(Player* player);
	void delete_player(uint64 sessionId);


protected:
	unordered_map<uint64, Player*> _players;
	GameServer* _server;
};

