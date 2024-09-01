#pragma once

#include "DBWriter.h"
#include "GameObject.h"
#include "PacketHandler.h"
#include "SqlSession.h"
#include "mysqlx/xdevapi.h"


namespace game
{
	struct CharacterInfo;
	class Player;

	template <typename T>
	struct Node;

	class Character : public GameObject, enable_object_pooling
	{
		friend Player;
		REGISTER_INHERITANCE(Character)

	public:
		class Factory
		{
			inline static ObjectPoolTls<Character> _pool;
			inline static uint64 _idGen = 0;
			inline static Lock _lock;

			static Factory& Instance();
		public:
			static void Create(CharacterInfo* info, mysqlx::Row& queryResult);
			static void Create(CharacterInfo* info, uint64 playerId, int32 idx, const wstring& nickName, int32 modelId, int32 weaponId);
			static void Destroy(Character* ptr);
		};

		Character(uint64 playerId, CharacterInfo& info);
		void OnPathFindingCompletion() override;

	protected:
		void OnUpdate() override;
		void OnSpawnRequest(const list<GameHost*>& sessionList) override;
		void OnDestroyRequest(const list<GameHost*>& sessionList) override;

	private:
		uint64	_id;
		uint64	_playerId;
		int32	_idx;
		wstring _nickname;
		int32	_level;
		int32	_exp;
		int32	_maxHp;
		int32	_hp;
		int32	_maxMp;
		int32	_mp;
		int32	_speed;
		int32	_modelId;
		int32	_weaponId;
		int32	_fieldId;

	public:
		/// Getter
		int32			MaxHp() const { return _maxHp; }
		int32			MaxMp() const { return _maxMp; }
		int32			Mp() const { return _mp; }
		uint64			Id() const { return _id; }
		uint64			PlayerId() const { return _playerId; }
		int32			Idx() const { return _idx; }
		const wstring& Nickname() const { return _nickname; }
		int32			Level() const { return _level; }
		int32			Exp() const { return _exp; }
		int32			Hp() const { return _hp; }
		int32			Speed() const { return _speed; }
		int32			ModelId() const { return _modelId; }
		int32			WeaponId() const { return _weaponId; }
		int32			FieldId() const { return _fieldId; }

		/// Set property
		void			SetHp(int32 val){_hp = val;}
		void			SetFieldId(int32 id){_fieldId = id;}
		int32			IncreaseExp(int32 val);
		void			DecreaseHp(int32 amount);
	};
}
