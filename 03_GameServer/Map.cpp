#include "stdafx.h"
#include "Map.h"
#include "GroupBase.h"
#include "PacketHandler.h"
#include "Player.h"

Map::Map(GroupBase* group, int32 width, int32 height, int32 sectorWidth, int32 sectorHeight)
	: _width(width)
	, _height(height)
	, _sectorWidth(sectorWidth)
	, _sectorHeight(sectorHeight)
	, _sectorMaxY(_height / sectorHeight)
	, _sectorMaxX(_width / sectorWidth)
	, _sectors(_sectorMaxY, vector<SectorInfo>(_sectorMaxX))
	, _group(group)
{

	for (int32 y = 0; y < _sectorMaxY; y++)
	{
		for (int32 x = 0; x < _sectorMaxX; x++)
		{
			_sectors[y][x].sector = new Sector;
		}
	}

	for (int32 y = 0; y < _sectorMaxY; y++)
	{
		for (int32 x = 0; x < _sectorMaxX; x++)
		{
			SetAroundSectors(y, x);
		}
	}
}

bool Map::MoveSector(Player& player)
{
	Character* character = player.curCharacter;
	Sector* newSector = FindSectorByPosition(character->Y(), character->X());
	if (newSector == player.sector)
	{
		return false;
	}

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

	SendPacket(player, destroySectorRange, Make_S_DESTROY_CHARACTER(player.id), false);

	SectorList sectors;
	GetSectors(sectors, player.sector->pos.y, player.sector->pos.x, destroySectorRange);
	for (Sector* sector : sectors)
	{
		for (Player* p : sector->players)
		{
			_group->SendPacket(player.sessionId, Make_S_DESTROY_CHARACTER(p->id));
		}
	}
	sectors.clear();

	player.sector->players.erase(&player);
	player.sector = newSector;
	player.sector->players.insert(&player);

	SendPacket(player, spawnSectorRange,
		Make_S_SPAWN_CHARACTER(
			player.id,
			character->Nickname(),
			character->Level(),
			character->Y(),
			character->X(),
			character->Hp(),
			character->Speed(),
			character->ModelId(),
			character->WeaponId(),
			character->Rotation()
		), false);
	SendPacket(player, spawnSectorRange, Make_S_MOVE_OTHER(player.id, character->Target().y(), character->Target().x()), false);

	GetSectors(sectors, player.sector->pos.y, player.sector->pos.x, spawnSectorRange);

	for (Sector* sector : sectors)
	{
		for (Player* p : sector->players)
		{
			Character* curCharacter = p->curCharacter;

			_group->SendPacket(player.sessionId,
				Make_S_SPAWN_CHARACTER(
					p->id,
					curCharacter->Nickname(),
					curCharacter->Level(),
					curCharacter->Y(),
					curCharacter->X(),
					curCharacter->Hp(),
					curCharacter->Speed(),
					curCharacter->ModelId(),
					curCharacter->WeaponId(),
					curCharacter->Rotation()
				));

			if (curCharacter->Target() != curCharacter->Pos())
			{
				_group->SendPacket(player.sessionId, Make_S_MOVE_OTHER(p->id, curCharacter->Target().y(), curCharacter->Target().x()));
			}
		}
	}

	return true;
}

void Map::SetAroundSectors(int32 y, int32 x)
{
	SectorInfo& info = _sectors[y][x];
	info.sector->pos = { y, x };

	for (int d = 0; d < 8; d++)
	{
		Point np = { y + dy[d], x + dx[d] };

		if (np.y < 0 || np.y >= _sectorMaxY || np.x < 0 || np.x >= _sectorMaxX)
		{
			continue;
		}

		info.aroundSectors[d] = _sectors[np.y][np.x].sector;
	}

	info.aroundSectors[8] = info.sector;

}

void Map::SendPacket(Player& player, int32 sectorRange, Packet& pkt, bool sendToOwn)
{

	Player* except = nullptr;

	if (sendToOwn == false)
	{
		except = &player;
	}

	Point pos = player.sector->pos;

	int32 currentSector = 1;

	pkt.IncRef();

	for (int32 idx = 0; idx < 9; idx++, currentSector <<= 1)
	{
		if ((sectorRange & currentSector) != 0)
		{
			Sector* sector = _sectors[pos.y][pos.x].aroundSectors[idx];
			if (sector == nullptr)
			{
				continue;
			}

			unordered_set<Player*>& players = _sectors[pos.y][pos.x].aroundSectors[idx]->players;

			for (Player* p : players)
			{
				if (p != except)
				{
					_group->SendPacket(p->sessionId, pkt);
				}
			}
		}
	}

	pkt.DecRef();
}

void Map::GetSectors(SectorList& list, int32 y, int32 x, int32 sectorRange)
{

	int32 currentSector = 1;
	for (int32 idx = 0; idx < 9; idx++, currentSector <<= 1)
	{
		if ((sectorRange & currentSector) != 0)
		{
			Sector* sector = _sectors[y][x].aroundSectors[idx];
			if (sector == nullptr)
			{
				continue;
			}

			list.push_back(sector);
		}
	}

}
