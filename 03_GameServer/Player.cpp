#include "stdafx.h"
#include "Player.h"

#include "Character.h"


Player::Player()
{
	characterInfos.clear();
	characterInfos.resize(6);
}

CharacterInfo& Player::AddCharacter(mysqlx::Row& queryResult)
{
	//Character* character = Character::Factory::Create(queryResult);

	int32 index = queryResult[1].get<int32>();
	Character::Factory::Create(&characterInfos[index], queryResult);
	numCharacters++;

	return characterInfos[index];
}

CharacterInfo& Player::AddCharacter(const wstring& nickname, int32 modelId, int32 weaponId)
{
	int32 index = numCharacters;
	Character::Factory::Create(&characterInfos[index], id, numCharacters, nickname, modelId, weaponId);
	numCharacters++;


	return characterInfos[index];
}
