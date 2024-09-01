#pragma once
#include "GameObject.h"


namespace game
{
	struct MonsterInfo;
	class Character;
	class Sector;

	class Monster : public GameObject, public enable_object_pooling
	{
		REGISTER_INHERITANCE(Monster)
	public:

		Monster(uint64 id, const MonsterInfo* info);
		void SetSpawnPoint(const Position& point, int32 sectionId)
		{
			_spawnPoint = point;
			_sectionId = sectionId;
		}

		const Position& GetSpawnPoint() { return _spawnPoint; }
		int32 GetSectionId() const { return _sectionId; }

		void OnUpdate() override;
		void OnSpawnRequest(const list<GameHost*>& sessionList) override;
		void OnDestroyRequest(const list<GameHost*>& sessionList) override;

	public:
		void OnPathFindingCompletion() override;

		const MonsterInfo* info;

		uint64 id;
		uint64 monsterId;
		int32 maxHp;
		int32 hp;
		int32 attackDamage;
		float32 attackRange;
		float32 attackSpeed;
		float32 castTime;
		int32 speed;
		int32 giveExp;
		Character* target = nullptr;


	private:
		float32 _attackTimer;
		float32 _followTimer = .5f;
		bool	_isAttack = false;
		Position _spawnPoint;
		int32	_sectionId;
		bool _isReservedPathFinding = false;
	};

}

