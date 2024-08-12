#pragma once
#include "GameObject.h"


class FieldManager : FixedObject
{

	inline static uint64 monsterIdGenerator = 0;

public:
	FieldManager() {}
	~FieldManager() override;
	void SetSpawnPoint(uint64 id, const Position& spawnPoint);
	void DestroyMonster(Monster* monster);
	Monster* FindMonster(uint64 id);

protected:
	void OnUpdate() override;


private:
	unordered_map<uint64, Monster*> _monsters;
};

