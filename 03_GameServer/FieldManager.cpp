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
	monster->SetSpawnPoint(spawnPoint);
	_monsters.insert({ uid, monster });

}

void FieldManager::DestroyMonster(Monster* monster)
{
	Position spawnPoint = monster->GetSpawnPoint();

	Invoke([this, spawnPoint]()
		{
			uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);
			Monster* newMonster = CreateObject<Monster>(spawnPoint, uid, 10, 2, 100);
			newMonster->SetSpawnPoint(spawnPoint);
			_monsters.insert({ uid, newMonster });

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
