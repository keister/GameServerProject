#include "stdafx.h"
#include "Character.h"

#include "SqlSession.h"
#include <numbers>

using namespace Eigen;

Character::Factory& Character::Factory::Instance()
{
	static Factory* instance = nullptr;

	if (instance == nullptr)
	{
		WRITE_LOCK(_lock);
		if (instance == nullptr)
		{
			auto ret = GetSqlSession().getSchema("game").getTable("character").select("MAX(id)").execute();
			mysqlx::Row row = ret.fetchOne();
			if (!row[0].isNull())
			{
				_idGen = row[0].get<uint64>() + 1;
			}
		}
	}

	return *instance;
}

Character* Character::Factory::Create(mysqlx::Row& queryResult)
{
	Character* ret = Instance()._pool.Alloc(queryResult);

	return ret;
}

Character* Character::Factory::Create(uint64 playerId, int32 idx, const wstring& nickName, int32 modelId, int32 weaponId)
{
	uint64 id = InterlockedIncrement64((LONGLONG*)&_idGen) - 1;

	Character* ret = Instance()._pool.Alloc(id, playerId, idx, nickName, modelId, weaponId);

	return ret;
}

void Character::Factory::Destroy(Character* ptr)
{
	Instance()._pool.Free(ptr);
}

Character::Character(mysqlx::Row& queryResult)
{
	_id = queryResult[0].get<uint64>();
	_playerId = queryResult[1].get<uint64>();
	_idx = queryResult[2].get<int32>();
	_nickname = queryResult[3].get<wstring>();
	_level = queryResult[4].get<int32>();
	_exp = queryResult[5].get<int32>();
	_pos.y() = queryResult[6].get<float32>();
	_pos.x() = queryResult[7].get<float32>();
	_hp = queryResult[8].get<int32>();
	_speed = queryResult[9].get<int32>();
	_modelId = queryResult[10].get<int32>();
	_weaponId = queryResult[11].get<int32>();
	_fieldId = queryResult[12].get<int32>();

	_target = _pos;
	_rotation = 0;
}

Character::Character(uint64 id, uint64 playerId, int32 idx, const wstring& nickname, int32 modelId, int32 weaponId)
{
	_id = id;
	_idx = idx;
	_playerId = playerId;
	_nickname = nickname;
	_modelId = modelId;
	_weaponId = weaponId;

	_level = 1;
	_exp = 0;
	_pos.y() = 0;
	_pos.x() = 0;
	_hp = 100;
	_speed = 1;
	_fieldId = 0;

	_target = _pos;
	_rotation = 0;
}

void Character::MoveTowards(float32 Delta)
{
	Vector2<float32> dir = _target - _pos;
	float32 distance = dir.norm();
	if (distance == 0.0f)
	{
		return;
	}

	dir = dir / distance;

	float moveDistance = fmin(Delta * _speed, distance);

	_pos += dir * moveDistance;
	_rotation = atan2(dir.x(), dir.y()) * (180.0f / numbers::pi_v<float>);
}


