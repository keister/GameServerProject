#include "stdafx.h"
#include "Monster.h"

#include "Character.h"
#include "ObjectType.h"
#include "PacketHandler.h"


Monster::Monster(uint64 id, int32 attackDamage, int32 speed,int32 maxHp)
	: id(id), attackDamage(attackDamage), speed(speed), hp(maxHp), maxHp(maxHp)
{
	

}

void Monster::OnUpdate()
{
	if (target != nullptr)
	{
		MoveTowards(target->position, speed * DeltaTime());
	}

	Packet movePacket = Make_S_MOVE_OBJECT(
		(uint8)ObjectType::MONSTER,
		id,
		position.y(),
		position.x()
	);
	SendPacket(AROUND, movePacket);
}

void Monster::OnSpawnRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_SPAWN_MONSTER(
		id,
		monsterId,
		hp,
		speed,
		position.y(),
		position.x()
	);

	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
	}
}

void Monster::OnDestroyRequest(const list<GameHost*>& sessionList)
{
	Packet pkt = Make_S_DESTROY_OBJECT((uint8)ObjectType::MONSTER, id);

	for (GameHost* host : sessionList)
	{
		SendPacket(host->SessionId(), pkt);
	}
}
