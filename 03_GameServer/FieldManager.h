#pragma once
#include "GameObject.h"

namespace game
{
	/// @brief 필드의 몬스터를 관리하는 클래스
	class FieldManager : FixedObject
	{

		inline static uint64 monsterIdGenerator = 0;

	public:
		FieldManager() {}
		~FieldManager() override;
		void		SetSpawnPoint(uint64 id, const Position& spawnPoint, int32 section);
		void		DestroyMonster(Monster* monster);
		Monster*	FindMonster(uint64 id);


	private:
		unordered_map<uint64, Monster*> _monsters;
	};
}
