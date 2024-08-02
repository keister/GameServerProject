#include "stdafx.h"
#include "FieldManager.h"

#include "Monster.h"

FieldManager::~FieldManager()
{
}

void FieldManager::SetSpawnPoint(uint64 id, const Position& spawnPoint)
{
	uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);

	Monster* monster = CreateObject<Monster>(spawnPoint, uid, 10, 2, 100);
	_monsters.insert({ uid, monster });

}

void FieldManager::DestroyMonster(Monster* monster)
{
	Invoke([this, monster]()
		{
			uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);
			_monsters.insert({ uid, CreateObject<Monster>(monster->position, uid, 10, 2, 100) });
			
		}, 5.f);

	_monsters.erase(monster->id);
	DestroyObject(monster);
}

Monster* FieldManager::FindMonster(uint64 id)
{
	auto it = _monsters.find(id);

	if (it == _monsters.end())
	{
		return nullptr;
	}

	return it->second;
}

void FieldManager::OnUpdate()
{
	FixedObject::OnUpdate();
}
