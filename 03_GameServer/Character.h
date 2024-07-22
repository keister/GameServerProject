#pragma once

#include "mysqlx/xdevapi.h"
#include <Dense>
class Player;

class Character
{
	friend Player;

public:
	class Factory
	{
		inline static ObjectPoolTls<Character> _pool;
		inline static uint64 _idGen = 0;
		inline static Lock _lock;

		static Factory& Instance();
	public:
		static Character* Create(mysqlx::Row& queryResult);
		static Character* Create(uint64 playerId, int32 idx, const wstring& nickName, int32 modelId, int32 weaponId);
		static void Destroy(Character* ptr);
	};

	Character(mysqlx::Row& queryResult);
	Character(uint64 id, uint64 playerId, int32 idx, const wstring& nickname, int32 modelId, int32 weaponId);

	void Move(float32 y, float32 x)
	{
		_target = { x, y };

	}


	void MoveTowards(float32 maxDistance);

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

	Eigen::Vector2<float32> _pos;
	Eigen::Vector2<float32> _target;
	float32 _rotation;



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

	float32 Y() const
	{
		return _pos.y();
	}

	float32 X() const
	{
		return _pos.x();
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
	Eigen::Vector2<float32>& Pos()
	{
		return _pos;
	}

	float32 Rotation()
	{
		return _rotation;
	}
};
