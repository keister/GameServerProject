#include "stdafx.h"
#include "GameGroup.h"

#include "Character.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "Player.h"
#include "Map.h"
#include "FieldManager.h"
#include "MapData.h"
#include "Monster.h"
#include "ObjectType.h"
#include "PathFindingTestObject.h"
#include "PathReceiver.h"

void game::GameGroup::OnRecv(uint64 sessionId, Packet packet)
{
	Player* player = FindPlayer(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_GameGroup(this, *player, packet);
}

void game::GameGroup::Handle_C_MOVE(Player& player, float32 y, float32 x)
{
	Character* character = player.curCharacter;

	character->GetPathReciever().RequestPathFinding({ x, y });
}

void game::GameGroup::Handle_C_SKILL(Player& player, uint64 objectId, uint32 skillId)
{
	Character* character = player.curCharacter;
	Monster* monster = _field->FindMonster(objectId);
	character->GetPathReciever().IgnorePreviousPath();
	if (monster == nullptr)
	{
		character->SendPacket(AROUND, Make_S_SKILL(player.id, skillId, character->position.y(), character->position.x(), character->position.y(), character->position.x()));
		return;
	}

	character->SendPacket(AROUND, Make_S_SKILL(player.id, skillId, monster->position.y(), monster->position.x(), character->position.y(), character->position.x()));

	float32 distance = (character->position - monster->position).norm();

	if (distance <= 2)
	{
		if (monster->target == nullptr)
		{
			monster->target = player.curCharacter;
		}

		monster->Invoke([monster, damage = character->Level() * 5]
			{
				int32 hp = monster->hp -= damage;
				monster->SendPacket(AROUND, Make_S_DAMAGE((uint8)ObjectType::MONSTER, monster->id, hp));
			}, 0.8f);

		if (monster->hp - character->Level() * 5 <= 0)
		{
			character->Invoke([this, sessionId = player.SessionId(), monster, character, monsterId = monster->id]
				{
					character->IncreaseExp(monster->giveExp);
					SendPacket(sessionId, Make_S_EXP(character->Exp()));
					if (monster->id == monsterId)
						_field->DestroyMonster(monster);
				}, 0.8f);
		}
	}
}

void game::GameGroup::Handle_C_MOVE_FIELD(Player& player, uint8 fieldId)
{
	if (player.curCharacter->position.x() <= 30.f)
	{
		player.spawnInfo = { 50.f, 48.f };
	}
	else
	{
		player.spawnInfo = { 10.f, 10.f };
	}


	MoveGroup(player.SessionId(), fieldId);
	Packet pkt = Make_S_MOVE_FIELD(fieldId);
	SendPacket(player.SessionId(), pkt);
}

void game::GameGroup::Handle_C_CHARACTER_RESPAWN(Player& player)
{
	Character* character = player.curCharacter;
	character->SetHp(character->MaxHp());

	Packet pkt = Make_S_CHARACTER_RESPAWN(player.id, character->Hp());

	character->SendPacket(AROUND, pkt);
}

game::GameGroup::GameGroup()
{

}

game::GameGroup::~GameGroup()
{
}

void game::GameGroup::OnPlayerEnter(Player& player)
{
	CharacterInfo* info = player.GetCurInfo();
	Character* character = CreateObject<Character>(player.spawnInfo, player.id, *info);
	//Character* character = CreateObject<Character>({0.f, 0.f}, player.id, *info);
	player.AttachObject(character);
	player.curCharacter = character;
	player.curCharacter->SetFieldId(GetGroupIndex());
	SendPacket(player.SessionId(), Make_S_SPAWN_CHARACTER(
		player.id,
		character->Nickname(),
		character->Level(),
		character->position.y(),
		character->position.x(),
		character->MaxHp(),
		character->Hp(),
		character->MaxMp(),
		character->Mp(),
		character->Speed(),
		character->ModelId(),
		character->WeaponId(),
		character->yaw
	));
	Packet SetExpPacket = Make_S_EXP(character->Exp());

	SendPacket(player.SessionId(), SetExpPacket);
	if (character->Hp() <= 0)
	{
		Packet deathPacket = Make_S_CHARACTER_DEATH(player.id);
		SendPacket(player.SessionId(), deathPacket);
	}


}

void game::GameGroup::OnPlayerLeave(Player& player)
{
	player.DetachObject();

	Character* character = player.curCharacter;

	CharacterInfo& info = player.characterInfos[character->Idx()];
	info = character;

	character->ExecuteForEachHost<Player>(AROUND, [this, &player, character](Player* p)
		{
			if (p == &player)
			{
				return;
			}

			SendPacket(p->SessionId(), Make_S_DESTROY_CHARACTER(player.id));
		});

	DBWriter::Write(Job::Alloc([
		id = character->Id(),
			level = character->Level(),
			exp = character->Exp(),
			position = character->position,
			hp = character->Hp(),
			mp = character->Mp(),
			fieidId = character->FieldId()
	]
		{
			SqlSession& sql = GetSqlSession();
			sql.getSchema("game").getTable("character")
				.update()
				.set("level", level)
				.set("exp", exp)
				.set("position_y", position.y())
				.set("position_x", position.x())
				.set("hp", hp)
				.set("mp", mp)
				.set("field_id", fieidId)
				.where("id = :ID")
				.bind("ID", id)
				.execute();
		}
	));

	DestroyObject(player.GetGameObject());
}

void game::GameGroup::Init()
{
	CreateMap(std::format("./Data/MapData/{}.mp", GetGroupIndex()).c_str(), 12, 12);

	_field = CreateFixedObject<FieldManager>();

	for (auto& spawnData : GetMap()->GetData()->GetMonsterSpawnPoint())
	{
		_field->SetSpawnPoint(spawnData.id, { spawnData.x, spawnData.y }, spawnData.sectionId);
	}
}
