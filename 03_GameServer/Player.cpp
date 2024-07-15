#include "stdafx.h"
#include "Player.h"

#include "Character.h"


Character* Player::AddCharacter(mysqlx::Row& queryResult)
{
	Character* character = Character::Factory::Create(queryResult);
	numCharacters++;
	characters[character->Idx()] = character;

	return character;
}

Character* Player::AddCharacter(const wstring& nickname, int32 modelId, int32 weaponId)
{
	Character* character = Character::Factory::Create(id, numCharacters, nickname, modelId, weaponId);
	numCharacters++;
	characters[character->Idx()] = character;

	return character;
}
