#pragma once
#include "GameObject.h"
class Character;
class Sector;

class Monster : public GameObject, public enable_object_pooling
{
	REGISTER_INHERITANCE(Monster)
public:


	Monster(uint64 id, int32 attackDamage, int32 speed, int32 maxHp);
	void SetSpawnPoint(const Position& point)
	{
		_spawnPoint = point;
	}

	const Position& GetSpawnPoint()
	{
		return _spawnPoint;
	}
	void OnUpdate() override;
	void OnSpawnRequest(const list<GameHost*>& sessionList) override;
	void OnDestroyRequest(const list<GameHost*>& sessionList) override;

protected:

	void MoveToSpawnPoint()
	{
		position = _spawnPoint;
	}

public:
	uint64 id;
	uint64 monsterId;
	int32 maxHp;
	int32 hp;
	int32 attackDamage;
	int32 speed;
	Character* target = nullptr;


private:
	float32 _attackTimer;
	Position _spawnPoint;
};

