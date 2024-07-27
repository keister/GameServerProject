#include "stdafx.h"
#include "GameGroup_Town.h"

#include "Character.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "Player.h"
#include "Map.h"
#include "Monster.h"
#include "ObjectType.h"
#include "Common/PacketDefine.h"

// void GameGroup_Town::OnEnter(uint64 sessionId)
// {
// 	Player* player = _server->FindPlayer(sessionId);
// 	insert_player(player);
//
// 	Character* character = player->curCharacter;
// 	player->sector = _map->FindSectorByPosition(character->Y(), character->X());
// 	SendPacket(player->sessionId, Make_S_GAME_ENTER(character->Id(), character->Nickname(), character->Level(), character->Exp(),
// 		character->Y(), character->X(), character->Hp(), character->Speed(), character->FieldId()));
//
// 	_map->SendPacket(*player, AROUND,
// 		Make_S_SPAWN_CHARACTER(
// 			player->id,
// 			character->Nickname(),
// 			character->Level(),
// 			character->Y(),
// 			character->X(),
// 			character->Hp(),
// 			character->Speed(),
// 			character->ModelId(),
// 			character->WeaponId(),
// 			character->Rotation()
// 		), false);
//
// 	const vector<Sector*>& around = _map->GetAroundSectors(player->sector);
//
// 	for (Sector* sector : around)
// 	{
// 		if (sector == nullptr)
// 		{
// 			continue;
// 		}
//
// 		for (Player* playerInSector : sector->players)
// 		{
// 			Character* playerCharater = playerInSector->curCharacter;
//
// 			SendPacket(player->sessionId, Make_S_SPAWN_CHARACTER(
// 				playerInSector->id,
// 				playerCharater->Nickname(),
// 				playerCharater->Level(),
// 				playerCharater->Y(),
// 				playerCharater->X(),
// 				playerCharater->Hp(),
// 				playerCharater->Speed(),
// 				playerCharater->ModelId(),
// 				playerCharater->WeaponId(),
// 				character->Rotation()
// 			));
// 		}
//
// 		for (Monster* monsterInSector : sector->monsters)
// 		{
// 			SendPacket(player->sessionId, Make_S_SPAWN_MONSTER(
// 				monsterInSector->id,
// 				monsterInSector->objectId,
// 				monsterInSector->hp,
// 				monsterInSector->speed,
// 				monsterInSector->pos.y(),
// 				monsterInSector->pos.x())
// 			);
// 		}
// 	}
//
// 	player->sector->players.insert(player);
//
//
// }
//
// void GameGroup_Town::OnLeave(uint64 sessionId)
// {
// 	Player* player = find_player(sessionId);
// 	_map->SendPacket(*player, AROUND, Make_S_DESTROY_CHARACTER(player->id), false);
//
// 	player->sector->players.erase(player);
//
// 	delete_player(sessionId);
// }

void GameGroup_Town::OnRecv(uint64 sessionId, Packet& packet)
{
	Player* player = find_player(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_GameGroup_Town(this, *player, packet);
}

// void GameGroup_Town::UpdateFrame()
// {
// 	for (auto p : _players)
// 	{
// 		Player* player = p.second;
//
// 		Character* character = player->curCharacter;
//
// 		character->MoveTowards(DeltaTime());
//
// 		_map->MoveSector(*player);
// 	}
// }

void GameGroup_Town::Handle_C_MOVE(Player& player, float32 y, float32 x)
{
	Character* character = player.curCharacter;
	character->Move(y, x);

	SendPacket(player.sessionId, Make_S_MOVE(y, x));
	_map->SendPacket(player, AROUND, Make_S_MOVE_OTHER(player.id, y, x), false);
}

void GameGroup_Town::Handle_C_SKILL(Player& player, uint64 objectId, uint32 skillId)
{
	Character* character = player.curCharacter;
	Monster* monster = _monsters.find(objectId)->second;
	character->Move(character->Y(), character->X());
	SendPacket(player.sessionId, Make_S_SKILL(skillId, monster->pos.y(), monster->pos.x(), character->Y(), character->X()));
	_map->SendPacket(player, AROUND, Make_S_SKILL_OTHER(player.id, skillId, monster->pos.y(), monster->pos.x(), character->Y(), character->X()), false);

	float32 distance = (character->Pos() - monster->pos).norm();

	

	if (distance <= 1.5)
	{
		int32 hp = monster->hp -= 5;

		Invoke([=, this, &player]()
		{
			_map->SendPacket(player, AROUND, Make_S_DAMAGE(objectId, hp), true);
		}
		, 400ul);
	}
}

GameGroup_Town::GameGroup_Town()
	: _map(new Map(this, 60, 60, 15, 15))
{

	Monster* monster = CreateObject<Monster>();

	DestroyObject(monster);

	//Monster* monster = Monster::AllocMonster(0);
	_monsters.insert({ monster->id, monster });
	monster->SetPos(10, 10);
	monster->SetSector(_map->FindSectorByPosition(monster->pos.y(), monster->pos.x()));
	monster->sector->monsters.insert(monster);
}

GameGroup_Town::~GameGroup_Town()
{
}
