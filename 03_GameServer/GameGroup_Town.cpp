#include "stdafx.h"
#include "GameGroup_Town.h"

#include "Character.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "Player.h"
#include "Map.h"
#include "FieldManager.h"
#include "Monster.h"
#include "ObjectType.h"
#include "Common/PacketDefine.h"

void GameGroup_Town::OnRecv(uint64 sessionId, Packet packet)
{
	Player* player = find_player(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_GameGroup_Town(this, *player, packet);
}

void GameGroup_Town::Handle_C_MOVE(Player& player, float32 y, float32 x)
{
	Character* character = player.curCharacter;
	character->SetTargetPos({ x, y });

	character->SendPacket(AROUND, Make_S_MOVE(player.id, y, x));
	//character->SendPacket(AROUND, Make_S_MOVE_OTHER(player.id, y, x), player.SessionId());
}

void GameGroup_Town::Handle_C_SKILL(Player& player, uint64 objectId, uint32 skillId)
{
	Character* character = player.curCharacter;
	Monster* monster = _field->FindMonster(objectId);
	character->SetTargetPos(character->position);
	if (monster == nullptr)
	{
		character->SendPacket(AROUND, Make_S_SKILL(player.id, skillId, character->position.y(), character->position.x(), character->position.y(), character->position.x()));

		//character->SendPacket(AROUND, Make_S_SKILL_OTHER(player.id, skillId, character->position.y(), character->position.x(), character->position.y(), character->position.x()), player.SessionId());
		return;
	}

	//Eigen::Vector2<float32> targetPos = { 0, 0 };


	character->SendPacket(AROUND, Make_S_SKILL(player.id, skillId, monster->position.y(), monster->position.x(), character->position.y(), character->position.x()));

	//character->SendPacket(AROUND, Make_S_SKILL_OTHER(player.id, skillId, monster->position.y(), monster->position.x(), character->position.y(), character->position.x()),player.SessionId());

	float32 distance = (character->position - monster->position).norm();



	if (distance <= 1.5)
	{
		int32 hp = monster->hp -= 5;
		if (monster->target == nullptr)
		{
			monster->target = player.curCharacter;
		}
		monster->SendPacket(AROUND, Make_S_DAMAGE((uint8)ObjectType::MONSTER, objectId, hp));

		if (monster->hp <= 0)
		{
			character->IncreaseExp(6);
			SendPacket(player.SessionId(), Make_S_EXP(character->Exp()));
			_field->DestroyMonster(monster);
		}
	}
}

GameGroup_Town::GameGroup_Town()
{

	CreateMap(60, 60, 15, 15);

	_field = CreateFixedObject<FieldManager>();

	_field->SetSpawnPoint(1, { 20.f, 20.f });
	_field->SetSpawnPoint(1, { 25.f, 20.f });
	_field->SetSpawnPoint(1, { 24.f, 24.f });
	_field->SetSpawnPoint(1, { 18.f, 29.f });


	// DestroyObject(monster);
	//
	// //Monster* monster = Monster::AllocMonster(0);
	// _monsters.insert({ monster->id, monster });
	// monster->SetPos(10, 10);
	// monster->SetSector(_map->FindSectorByPosition(monster->pos.y(), monster->pos.x()));
	//monster->sector->monsters.insert(monster);
}

GameGroup_Town::~GameGroup_Town()
{
}

void GameGroup_Town::OnPlayerEnter(Player& player)
{
	CharacterInfo* info = player.GetCurInfo();

	Character* character = CreateObject<Character>(info->pos, player.id, *info);
	player.AttachObject(character);
	player.curCharacter = character;

	SendPacket(player.SessionId(), Make_S_GAME_ENTER(
		player.id,
		character->Id()
	));

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
}

void GameGroup_Town::OnPlayerLeave(Player& player)
{
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

	DestroyObject(player.GetGameObject());
}
