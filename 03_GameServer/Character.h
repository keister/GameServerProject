#pragma once

#include "GameObject.h"
#include "mysqlx/xdevapi.h"
struct CharacterInfo;
class Player;

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

	void SetTargetPos(const Eigen::Vector2<float32>& targetPos)
	{
		_target = targetPos;
	}

private:
	uint64	_id;
	uint64	_playerId;
	int32	_idx;
	wstring _nickname;
	int32	_level;
	int32	_exp;
	int32	_hp;
	int32	_speed;
	int32	_modelId;
	int32	_weaponId;
	int32	_fieldId;

	Eigen::Vector2<float32> _target;


public:
	uint64 Id() const
	{
		return _id;
	}

	uint64 PlayerId() const
	{
		return _playerId;
	}

	int32 Idx() const
	{
		return _idx;
	}

	const wstring& Nickname() const
	{
		return _nickname;
	}

	int32 Level() const
	{
		return _level;
	}

	int32 Exp() const
	{
		return _exp;
	}

	int32 Hp() const
	{
		return _hp;
	}

	int32 Speed() const
	{
		return _speed;
	}

	int32 ModelId() const
	{
		return _modelId;
	}

	int32 WeaponId() const
	{
		return _weaponId;
	}

	int32 FieldId() const
	{
		return _fieldId;
	}

	const Eigen::Vector2<float32>& Target()
	{
		return _target;
	}

protected:
	void OnUpdate() override;
	void OnSpawnRequest(const list<GameHost*>& sessionList) override;
	void OnDestroyRequest(const list<GameHost*>& sessionList) override;
};
