#pragma once
#include <mysqlx/devapi/common.h>
#include <mysqlx/devapi/row.h>
class Sector;
class Character;

class Player
{
public:
	Player() = delete;
	Player(uint64 sessionId) : sessionId(sessionId)
	{
		characters.reserve(6);
		numCharacters = 0;
		curCharacter = nullptr;
	}


	Character* AddCharacter(mysqlx::Row& queryResult);
	Character* AddCharacter(const wstring& nickname, int32 modelId, int32 weaponId);

public:
	uint64 sessionId;
	uint64 id;
	uint64 accountId;
	vector<Character*> characters;
	int32		numCharacters;

	Character*	curCharacter;
	Sector*		sector;
};

