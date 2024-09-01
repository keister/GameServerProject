#include "stdafx.h"
#include "FieldManager.h"

#include "Monster.h"
#include "MonsterData.h"

game::FieldManager::~FieldManager()
{
}

/// @brief �ش� ������ ���� ���� ����Ʈ�� �����Ѵ�.
/// @param id			������ ������ id
/// @param spawnPoint	������ġ
/// @param section		Ȱ���� �� �ִ� ����
void game::FieldManager::SetSpawnPoint(uint64 id, const Position& spawnPoint, int32 section)
{
	// ���� id ����
	uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);

	// ���� ������ �ε�
	const MonsterInfo& info = MonsterData::Get(id);

	Monster* monster = CreateObject<Monster>(spawnPoint, uid, &info);
	monster->SetSpawnPoint(spawnPoint, section);
	_monsters.insert({ uid, monster });

}

void game::FieldManager::DestroyMonster(Monster* monster)
{
	// ���� �ı��� 5�ʵڿ� �ش���� ������ġ�� �����
	Invoke([this, spawnPoint = monster->GetSpawnPoint(), section = monster->GetSectionId(), info = monster->info]()
		{
			uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);
			Monster* newMonster = CreateObject<Monster>(spawnPoint, uid, info);
			newMonster->SetSpawnPoint(spawnPoint, section);
			_monsters.insert({ uid, newMonster });

		}, 5.f);

	_monsters.erase(monster->id);
	DestroyObject(monster);
}

game::Monster* game::FieldManager::FindMonster(uint64 id)
{
	auto it = _monsters.find(id);
	if (it == _monsters.end())
	{
		return nullptr;
	}

	return it->second;
}

