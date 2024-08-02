#pragma once
#include "GameObject.h"
class Character;
class Sector;

class Monster : public GameObject, public enable_object_pooling
{
	REGISTER_INHERITANCE(Monster)
public:


	Monster(uint64 id, int32 attackDamage, int32 speed, int32 maxHp);

protected:
	void OnUpdate() override;
	void OnSpawnRequest(const list<GameHost*>& sessionList) override;
	void OnDestroyRequest(const list<GameHost*>& sessionList) override;
	//~Monster() override;

	//void OnUpdate() override;
	//void OnSectorLeave(int32 sectorRange) override;
	//void OnSectorEnter(int32 sectorRange) override;

public:
	uint64 id;
	uint64 monsterId;
	int32 maxHp;
	int32 hp;
	int32 attackDamage;
	int32 speed;

	Character* target = nullptr;
};

