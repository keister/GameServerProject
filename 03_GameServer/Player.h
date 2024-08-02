#pragma once
#include <mysqlx/devapi/common.h>
#include <mysqlx/devapi/row.h>

#include "Character.h"
#include "GameHost.h"
class Sector;
class Character;

struct CharacterInfo
{
	uint64	id;
	int32	idx;
	wstring nickname;
	int32	level;
	int32	exp;
	Eigen::Vector2<float32> pos;
	int32	hp;
	int32	speed;
	int32	modelId;
	int32	weaponId;
	int32	fieldId;


	CharacterInfo& operator=(const Character* character)
	{
		level = character->Level();
		exp = character->Exp();
		pos = character->position;
		hp = character->Hp();
		speed = character->Speed();
		modelId = character->ModelId();
		weaponId = character->WeaponId();

		return *this;
	}
};

class Player : public GameHost
{
public:
	Player();

	CharacterInfo& AddCharacter(mysqlx::Row& queryResult);
	CharacterInfo& AddCharacter(const wstring& nickname, int32 modelId, int32 weaponId);

	CharacterInfo* GetCurInfo()
	{
		if (curIndex == -1)
		{
			return nullptr;
		}
		else
		{
			return &characterInfos[curIndex];
		}
	}

public:
	uint64 id;
	uint64 accountId;

	int32 numCharacters;
	vector<CharacterInfo> characterInfos;
	int32 curIndex = -1;
	Character*	curCharacter;
};

