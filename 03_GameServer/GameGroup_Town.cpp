#include "stdafx.h"
#include "GameGroup_Town.h"

#include "Character.h"
#include "GameServer.h"
#include "PacketHandler.h"
#include "Player.h"
#include "Map.h"

void GameGroup_Town::OnEnter(uint64 sessionId)
{
	Player* player = _server->FindPlayer(sessionId);
	insert_player(player);

	Character* character = player->curCharacter;
	player->sector = _map->FindSectorByPosition(character->Y(), character->X());
	SendPacket(player->sessionId, Make_S_GAME_ENTER(character->Id(), character->Nickname(), character->Level(), character->Exp(),
		character->Y(), character->X(), character->Hp(), character->Speed(), character->FieldId()));

	_map->SendPacket(*player, AROUND,
		Make_S_SPAWN_CHARACTER(
			player->id,
			character->Nickname(),
			character->Level(),
			character->Y(),
			character->X(),
			character->Hp(),
			character->Speed(),
			character->ModelId(),
			character->WeaponId()
		), false);

	const vector<Sector*>& around = _map->GetAroundSectors(player->sector);

	for (Sector* sector : around)
	{
		if (sector == nullptr)
		{
			continue;
		}

		for (Player* playerInSector : sector->players)
		{
			Character* playerCharater = playerInSector->curCharacter;

			SendPacket(player->sessionId, Make_S_SPAWN_CHARACTER(
				playerInSector->id,
				playerCharater->Nickname(),
				playerCharater->Level(),
				playerCharater->Y(),
				playerCharater->X(),
				playerCharater->Hp(),
				playerCharater->Speed(),
				playerCharater->ModelId(),
				playerCharater->WeaponId()
			));
		}
	}

	player->sector->players.insert(player);


}

void GameGroup_Town::OnLeave(uint64 sessionId)
{
	Player* player = find_player(sessionId);
	_map->SendPacket(*player, AROUND, Make_S_DESTROY_CHARACTER(player->id), false);

	player->sector->players.erase(player);

	delete_player(sessionId);
}

void GameGroup_Town::OnRecv(uint64 sessionId, Packet& packet)
{
	Player* player = find_player(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_GameGroup_Town(this, *player, packet);
}

void GameGroup_Town::UpdateFrame()
{
}

void GameGroup_Town::Handle_C_MOVE(Player& player, float32 y, float32 x)
{
	Character* character = player.curCharacter;
	character->Move(y, x);

	Sector* newSector = _map->FindSectorByPosition(character->Y(), character->X());

	if (newSector != player.sector)
	{
		int32 spawnSectorRange = 0;
		int32 destroySectorRange = 0;

		int32 dy = newSector->pos.y - player.sector->pos.y;
		int32 dx = newSector->pos.x - player.sector->pos.x;

		if (dx == 1)
		{
			spawnSectorRange |= COL_RIGHT;
			destroySectorRange |= COL_LEFT;
		}
		else if (dx == -1)
		{
			spawnSectorRange |= COL_LEFT;
			destroySectorRange |= COL_RIGHT;
		}

		if (dy == 1)
		{
			spawnSectorRange |= ROW_UP;
			destroySectorRange |= ROW_DOWN;
		}
		else if (dy == -1)
		{
			spawnSectorRange |= ROW_DOWN;
			destroySectorRange |= ROW_UP;
		}

		_map->SendPacket(player, destroySectorRange, Make_S_DESTROY_CHARACTER(player.id), false);

		player.sector = newSector;

		_map->SendPacket(player, spawnSectorRange, 
			Make_S_SPAWN_CHARACTER(
				player.id,
				character->Nickname(),
				character->Level(),
				character->Y(), 
				character->X(),
				character->Hp(),
				character->Speed(),
				character->ModelId(), 
				character->WeaponId()
			), false);
	}

	_map->SendPacket(player, AROUND, Make_S_MOVE(player.id, character->Y(), character->X()), false);
}

void GameGroup_Town::Handle_C_MOVE_STOP(Player& player, float32 y, float32 x)
{
	Character* character = player.curCharacter;
	character->Move(y, x);

	Sector* newSector = _map->FindSectorByPosition(character->Y(), character->X());

	if (newSector != player.sector)
	{
		int32 spawnSectorRange = 0;
		int32 destroySectorRange = 0;

		int32 dy = newSector->pos.y - player.sector->pos.y;
		int32 dx = newSector->pos.x - player.sector->pos.x;

		if (dx == 1)
		{
			spawnSectorRange |= COL_RIGHT;
			destroySectorRange |= COL_LEFT;
		}
		else if (dx == -1)
		{
			spawnSectorRange |= COL_LEFT;
			destroySectorRange |= COL_RIGHT;
		}

		if (dy == 1)
		{
			spawnSectorRange |= ROW_UP;
			destroySectorRange |= ROW_DOWN;
		}
		else if (dy == -1)
		{
			spawnSectorRange |= ROW_DOWN;
			destroySectorRange |= ROW_UP;
		}

		_map->SendPacket(player, destroySectorRange, Make_S_DESTROY_CHARACTER(player.id), false);

		player.sector = newSector;

		_map->SendPacket(player, spawnSectorRange,
			Make_S_SPAWN_CHARACTER(
				player.id,
				character->Nickname(),
				character->Level(),
				character->Y(),
				character->X(),
				character->Hp(),
				character->Speed(),
				character->ModelId(),
				character->WeaponId()
			), false);
	}

	_map->SendPacket(player, AROUND, Make_S_MOVE_STOP(player.id, character->Y(), character->X()), false);
}

GameGroup_Town::GameGroup_Town()
	: _map(new Map(this, 100, 100, 10, 10))
{
}

GameGroup_Town::~GameGroup_Town()
{
}
