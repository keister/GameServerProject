#pragma once

class Monster;

class SpawnManager
{
public:
	static Monster* AllocMonster(uint64 monsterId);
	static void DestroyMonster(Monster* ptr);
};

