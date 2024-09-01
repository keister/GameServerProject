#include "stdafx.h"
#include "FieldManager.h"

#include "Monster.h"
#include "MonsterData.h"

game::FieldManager::~FieldManager()
{
}

/// @brief 해당 지점을 몬스터 생성 포인트로 지적한다.
/// @param id			몬스터의 데이터 id
/// @param spawnPoint	스폰위치
/// @param section		활동할 수 있는 범위
void game::FieldManager::SetSpawnPoint(uint64 id, const Position& spawnPoint, int32 section)
{
	// 고유 id 생성
	uint64 uid = InterlockedIncrement64((LONG64*)&monsterIdGenerator);

	// 몬스터 데이터 로딩
	const MonsterInfo& info = MonsterData::Get(id);

	Monster* monster = CreateObject<Monster>(spawnPoint, uid, &info);
	monster->SetSpawnPoint(spawnPoint, section);
	_monsters.insert({ uid, monster });

}

void game::FieldManager::DestroyMonster(Monster* monster)
{
	// 몬스터 파괴시 5초뒤에 해당몬스터 스폰위치에 재생성
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

