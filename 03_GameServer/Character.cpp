#include "stdafx.h"
#include "Character.h"

#include "SqlSession.h"
#include <numbers>

#include "ObjectType.h"
#include "PacketHandler.h"
#include "PathReceiver.h"
#include "Player.h"

using namespace Eigen;

game::Character::Factory& game::Character::Factory::Instance()
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

/// @brief DB쿼리를 그대로 가져와서 CharacterInfo를 생성한다.
/// @param info  info를 받을 OUT 파라미터
/// @param queryResult db쿼리 결과
void game::Character::Factory::Create(CharacterInfo* info, mysqlx::Row& queryResult)
{
	info->id = queryResult[0].get<uint64>();
	info->idx = queryResult[1].get<int32>();
	info->nickname = queryResult[2].get<wstring>();
	info->level = queryResult[3].get<int32>();
	info->exp = queryResult[4].get<int32>();
	info->pos.y() = queryResult[5].get<float32>();
	info->pos.x() = queryResult[6].get<float32>();
	info->maxHp = queryResult[7].get<int32>();
	info->hp = queryResult[8].get<int32>();
	info->maxMp = queryResult[9].get<int32>();
	info->mp = queryResult[10].get<int32>();
	info->speed = queryResult[11].get<int32>();
	info->modelId = queryResult[12].get<int32>();
	info->weaponId = queryResult[13].get<int32>();
	info->fieldId = queryResult[14].get<int32>();
}

void game::Character::Factory::Create(CharacterInfo* info, uint64 playerId, int32 idx, const wstring& nickname, int32 modelId, int32 weaponId)
{
	uint64 idGen = Instance()._idGen;
	uint64 id = InterlockedIncrement64((LONGLONG*)&idGen) - 1;

	info->id = id;
	info->idx = idx;
	info->nickname = nickname;
	info->modelId = modelId;
	info->weaponId = weaponId;
	info->level = 1;
	info->exp = 0;
	info->pos.y() = 4;
	info->pos.x() = 10;
	info->hp = 100;
	info->maxHp = 100;
	info->maxMp = 100;
	info->mp = 100;
	info->speed = 3;
	info->fieldId = 0;

}

void game::Character::Factory::Destroy(Character* ptr)
{
	Instance()._pool.Free(ptr);
}

game::Character::Character(uint64 playerId, CharacterInfo& info)
{
	_id = info.id;
	_playerId = playerId;
	_idx = info.idx;
	_nickname = info.nickname;
	_modelId = info.modelId;
	_weaponId = info.weaponId;
	_level = info.level;
	_exp = info.exp;
	_maxHp = info.maxHp;
	_hp = info.hp;
	_maxMp = info.maxMp;
	_mp = info.mp;
	_speed = info.speed;
	_fieldId = info.fieldId;
}

void game::Character::DecreaseHp(int32 amount)
{
	_hp -= amount;

	Packet pkt = Make_S_DAMAGE((uint8)ObjectType::PLAYER, _playerId, _hp);

	SendPacket(AROUND, pkt);

	if (_hp <= 0)	// 사망
	{
		GetPathReciever().IgnorePreviousPath();
		Packet deathPacket = Make_S_CHARACTER_DEATH(_playerId);
		SendPacket(AROUND, deathPacket);
	}
}

void game::Character::OnUpdate()
{
	// 목적지에 도착했으면 정지
	if (GetPathReciever().CurrentTargetPosition() == GetPathReciever().GetRoute().end())
	{
		return;
	}

	// 현재 목적지 위치로 이동
	MoveTowards(*GetPathReciever().CurrentTargetPosition(), _speed * DeltaTime());

	// 현재 목적지에 도착했다면, 다음 목적지로 변경
	if (position == *GetPathReciever().CurrentTargetPosition())
	{
		GetPathReciever().SetNextTargetPosition();
	}
}

void game::Character::OnSpawnRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_SPAWN_CHARACTER(
		_playerId,
		_nickname,
		_level,
		position.y(),
		position.x(),
		_maxHp,
		_hp,
		_maxMp,
		_mp,
		_speed,
		_modelId,
		_weaponId,
		yaw
	);

	Packet movePacket = Make_S_MOVE(_playerId, GetPathReciever());

	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);

		if (_hp <= 0) // 죽은 상태이면 사망 패킷 보냄
		{
			Packet deathPacket = Make_S_CHARACTER_DEATH(_playerId);
			SendPacket(AROUND, deathPacket);
		}

		if (!GetPathReciever().GetRoute().IsEmpty()) // 움직이는 중이었다면 현재위치부터 남은 경로를 보냄
		{
			SendPacket(host->SessionId(), movePacket);
		}
	}

}

void game::Character::OnDestroyRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_DESTROY_CHARACTER(_playerId);
	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
	}
}

int32 game::Character::IncreaseExp(int32 val)
{
	_exp += val;
	if (_exp >= _level * 100)
	{
		_exp -= _level * 100;
		_level++;

		SendPacket(_host->SessionId(), Make_S_LEVEL_UP(_level));
	}


	return _exp;
}

void game::Character::OnPathFindingCompletion()
{
	Packet pkt = Make_S_MOVE(_playerId, GetPathReciever());

	SendPacket(AROUND, pkt);
}
