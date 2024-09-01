#include "stdafx.h"
#include "Monster.h"

#include "Character.h"
#include "MapData.h"
#include "MonsterData.h"
#include "ObjectType.h"
#include "PacketHandler.h"


game::Monster::Monster(uint64 id, const game::MonsterInfo* info)
	: id(id), info(info)
{
	monsterId = info->id;
	maxHp = info->hp;
	hp = info->hp;
	attackDamage = info->attackDamage;
	attackRange = info->attackRange;
	attackSpeed = info->attackSpeed;
	speed = info->speed;
	_attackTimer = 0.f;
	giveExp = info->giveExp;
	castTime = info->castTime;

	_followTimer = 0.f;
}

void game::Monster::OnUpdate()
{

	_attackTimer += DeltaTime();
	_followTimer -= DeltaTime();

	if (_isAttack == true)
	{
		return;
	}
	if (GetPathReciever().CurrentTargetPosition() != GetPathReciever().GetRoute().end())
	{
		MoveTowards(*GetPathReciever().CurrentTargetPosition(), speed * DeltaTime());

		if (GetGroup()->GetMap()->GetTileInfo(position) != TileInfo::MONSTER_SECTION)
		{
			if (target != nullptr)
			{
				target = nullptr;
				GetPathReciever().RequestPathFinding(GetGroup()->GetMap()->GetRandomPostionInSection(_sectionId));
			}
		}

		if (position == *GetPathReciever().CurrentTargetPosition())
		{
			GetPathReciever().SetNextTargetPosition();
		}

	}

	if (target != nullptr)
	{
		float32 distance = (position - target->position).norm();
		if (distance <= attackRange)
		{
			if (_attackTimer >= attackSpeed)
			{
				LookAt(target->position);

				Packet pkt = Make_S_MONSTER_ATTACK(id, position.y(), position.x(), yaw);
				SendPacket(AROUND, pkt);
				GetPathReciever().IgnorePreviousPath();
				_isAttack = true;
				Invoke([this]()
					{
						_isAttack = false;
					}, castTime);




				target->Invoke([character = target, attackDmg = attackDamage]
					{
						character->DecreaseHp(attackDmg);
					},
					castTime);

				if (target->Hp() - attackDamage <= 0)
				{
					target = nullptr;
				}

				_attackTimer = 0.f;
			}
		}
		else
		{
			if (_followTimer <= 0.f)
			{
				GetPathReciever().RequestPathFinding(target->position);
				_followTimer = .5f;
			}
		}
	}
	else
	{
		if (_isReservedPathFinding == false)
		{
			if ((GetPathReciever().CurrentTargetPosition() == GetPathReciever().GetRoute().end()))
			{
				_isReservedPathFinding = true;
				Invoke([this]
					{
						_isReservedPathFinding = false;
						if (target != nullptr)
						{
							return;
						}
						GetPathReciever().RequestPathFinding(GetGroup()->GetMap()->GetRandomPostionInSection(_sectionId));

					}, 3.f);
			}
		}

	}
}

void game::Monster::OnSpawnRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_SPAWN_MONSTER(
		id,
		monsterId,
		maxHp,
		hp,
		speed,
		position.y(),
		position.x()
	);
	Packet movePacket = Make_S_MOVE_OBJECT(
		(uint8)ObjectType::MONSTER,
		id,
		GetPathReciever()
	);

	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
		if (!GetPathReciever().GetRoute().IsEmpty())
		{
			SendPacket(host->SessionId(), movePacket);
		}
	}
}

void game::Monster::OnDestroyRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_DESTROY_OBJECT((uint8)ObjectType::MONSTER, id);

	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
	}
}

void game::Monster::OnPathFindingCompletion()
{
	Packet movePacket = Make_S_MOVE_OBJECT(
		(uint8)ObjectType::MONSTER,
		id,
		GetPathReciever()
	);
	SendPacket(AROUND, movePacket);
}
