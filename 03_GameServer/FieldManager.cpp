#include "stdafx.h"
#include "FieldManager.h"

#include "Monster.h"
#include "MonsterData.h"

FieldManager::~FieldManager()
{
}

void FieldManager::SetSpawnPoint(uint64 id, const Position& spawnPoint, int32 section)
{
	uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);

	const MonsterInfo& info = MonsterData::Get(id);

	Monster* monster = CreateObject<Monster>(spawnPoint, uid, &info);
	monster->SetSpawnPoint(spawnPoint, section);
	_monsters.insert({ uid, monster });

}

void FieldManager::DestroyMonster(Monster* monster)
{
	Position spawnPoint = monster->GetSpawnPoint();
	int32 section = monster->GetSectionId();
	const MonsterInfo* info = monster->info;

	Invoke([this, spawnPoint, section, info]()
		{
			uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);
			Monster* newMonster = CreateObject<Monster>(spawnPoint, uid, info);
			newMonster->SetSpawnPoint(spawnPoint, section);
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
