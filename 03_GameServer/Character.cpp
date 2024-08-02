#include "stdafx.h"
#include "Character.h"

#include "SqlSession.h"
#include <numbers>

#include "PacketHandler.h"
#include "Player.h"

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

void Character::Factory::Create(CharacterInfo* info, mysqlx::Row& queryResult)
{
	info->id = queryResult[0].get<uint64>();
	info->idx = queryResult[2].get<int32>();
	info->nickname = queryResult[3].get<wstring>();
	info->level = queryResult[4].get<int32>();
	info->exp = queryResult[5].get<int32>();
	info->pos.y() = queryResult[6].get<float32>();
	info->pos.x() = queryResult[7].get<float32>();
	info->hp = queryResult[8].get<int32>();
	info->speed = queryResult[9].get<int32>();
	info->modelId = queryResult[10].get<int32>();
	info->weaponId = queryResult[11].get<int32>();
	info->fieldId = queryResult[12].get<int32>();
}

void Character::Factory::Create(CharacterInfo* info, uint64 playerId, int32 idx, const wstring& nickname, int32 modelId, int32 weaponId)
{
	uint64 id = InterlockedIncrement64((LONGLONG*)&_idGen) - 1;

	info->id = id;
	info->idx = idx;
	info->nickname = nickname;
	info->modelId = modelId;
	info->weaponId = weaponId;
	info->level = 1;
	info->exp = 0;
	info->pos.y() = 0;
	info->pos.x() = 0;
	info->hp = 100;
	info->speed = 1;
	info->fieldId = 0;

}

void Character::Factory::Destroy(Character* ptr)
{
	Instance()._pool.Free(ptr);
}

Character::Character(uint64 playerId, CharacterInfo& info)
{
	_id = info.id;
	_playerId = playerId;
	_idx = info.idx;
	_nickname = info.nickname;
	_modelId = info.modelId;
	_weaponId = info.weaponId;
	_level = info.level;
	_exp = info.exp;
	_hp = info.hp;
	_speed = info.speed;
	_fieldId = info.fieldId;
	_target = position;
}

void Character::OnUpdate()
{
	MoveTowards(_target, _speed * DeltaTime());
}

void Character::OnSpawnRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_SPAWN_CHARACTER(
		_playerId,
		_nickname,
		_level,
		position.y(),
		position.x(),
		_hp,
		_speed,
		_modelId,
		_weaponId,
		yaw
	);

	Packet movePacket = Make_S_MOVE_OTHER(_playerId, _target.y(), _target.x());

	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
		if (_target != position)
		{
			SendPacket(host->SessionId(), movePacket);
		}
	}
}

void Character::OnDestroyRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_DESTROY_CHARACTER(_playerId);
	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
	}
}

// void Character::OnSectorLeave(int32 sectorRange)
// {
// 	SendPacket(sectorRange, );
// 	ExecuteForEachHost<Player>(sectorRange,
// 		[this](Player* p)
// 		{
// 			if (p == _host)
// 			{
// 				return;
// 			}
//
// 			SendPacket(_host->SessionId(), Make_S_DESTROY_CHARACTER(p->id));
// 		});
// }
//
// void Character::OnSectorEnter(int32 sectorRange)
// {
// 	SendPacket(sectorRange, Make_S_SPAWN_CHARACTER(
// 		_playerId,
// 		_nickname,
// 		_level,
// 		position.y(),
// 		position.x(),
// 		_hp,
// 		_speed,
// 		_modelId,
// 		_weaponId,
// 		yaw
// 	));
//
// 	// ExecuteForEachHost<Player>(sectorRange,
// 	// 	[this](Player* p)
// 	// 	{
// 	// 		if (p == _host)
// 	// 		{
// 	// 			return;
// 	// 		}
// 	// 		Character* curCharacter = p->curCharacter;
// 	//
// 	// 		SendPacket(_host->SessionId(), Make_S_SPAWN_CHARACTER(
// 	// 			p->id,
// 	// 			curCharacter->Nickname(),
// 	// 			curCharacter->Level(),
// 	// 			curCharacter->position.y(),
// 	// 			curCharacter->position.x(),
// 	// 			curCharacter->Hp(),
// 	// 			curCharacter->Speed(),
// 	// 			curCharacter->ModelId(),
// 	// 			curCharacter->WeaponId(),
// 	// 			curCharacter->yaw
// 	// 		));
// 	//
// 	// 		if (curCharacter->Target() != curCharacter->position)
// 	// 		{
//
// 	// 		}
// 	// 	});
// }
//
//
