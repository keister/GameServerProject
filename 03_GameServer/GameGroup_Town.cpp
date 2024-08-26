#include "stdafx.h"
#include "GameGroup_Town.h"

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

	character->GetPathReciever().RequestPathFinding({ x, y });

	//character->SendPacket(AROUND, Make_S_MOVE(player.id, y, x));
	//character->SendPacket(AROUND, Make_S_MOVE_OTHER(player.id, y, x), player.SessionId());
}

void GameGroup_Town::Handle_C_SKILL(Player& player, uint64 objectId, uint32 skillId)
{
	Character* character = player.curCharacter;
	Monster* monster = _field->FindMonster(objectId);
	character->GetPathReciever().IgnorePreviousPath();
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
		if (monster->target == nullptr)
		{
			monster->target = player.curCharacter;
		}
		monster->Invoke([monster]
			{
				int32 hp = monster->hp -= 5;
				monster->SendPacket(AROUND, Make_S_DAMAGE((uint8)ObjectType::MONSTER, monster->id, hp));
			}, 0.8f);

		if (monster->hp - 5 <= 0)
		{
			uint64 sessionId = player.SessionId();
			character->Invoke([this, sessionId, monster, character]
				{
					character->IncreaseExp(monster->giveExp);
					SendPacket(sessionId, Make_S_EXP(character->Exp()));
					_field->DestroyMonster(monster);
				}, 0.8f);
		}
	}
}

void GameGroup_Town::Handle_C_MOVE_FIELD(Player& player, uint8 fieldId)
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

GameGroup_Town::GameGroup_Town()
{


	// for (int32 i = 0; i < 100; i++)
	// {
	// 	CreateObject<PathFindingTestObject>({ 0, 0 });
	// }

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
	Character* character = CreateObject<Character>(player.spawnInfo, player.id, *info);
	//Character* character = CreateObject<Character>({0.f, 0.f}, player.id, *info);
	player.AttachObject(character);
	player.curCharacter = character;

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

	DestroyObject(player.GetGameObject());
}

void GameGroup_Town::Init()
{
	CreateMap(std::format("./{}.mp", GetGroupIndex()).c_str(), 15, 15);

	_field = CreateFixedObject<FieldManager>();

	for (auto& spawnData : GetMap()->GetData()->GetMonsterSpawnPoint())
	{
		_field->SetSpawnPoint(spawnData.id, { spawnData.x, spawnData.y }, spawnData.sectionId);
	}
}
