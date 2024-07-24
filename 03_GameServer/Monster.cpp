#include "stdafx.h"
#include "Monster.h"

Monster* Monster::AllocMonster(uint64 monsterId)
{
	static uint64 idGen = 0;

	Monster* monster = _pool.Alloc();
	monster->id = InterlockedIncrement64((LONGLONG*)&idGen);
	monster->target = nullptr;
	monster->attackDamage = 10;
	monster->objectId = 0;
	monster->speed = 3;
	monster->hp = 100;

	return monster;
}

void Monster::DestroyMonster(Monster* ptr)
{
	_pool.Free(ptr);
}
