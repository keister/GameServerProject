#pragma once
class Character;
class Sector;

class Monster
{
public:


	void SetPos(float32 y, float32 x)
	{
		pos = { x, y };
	}

	void SetSector(Sector* sector)
	{
		this->sector = sector;
	}

	static Monster* AllocMonster(uint64 monsterId);
	static void DestroyMonster(Monster* ptr);


public:
	uint64 id;
	uint64 objectId;
	int32 hp;
	int32 attackDamage;
	int32 speed;
	Sector* sector;
	Eigen::Vector2<float32> pos;
	Character* target = nullptr;

	inline static ObjectPoolTls<Monster, false> _pool;
};

