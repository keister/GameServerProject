#include "stdafx.h"
#include "Map.h"
#include "GroupBase.h"
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

void Map::SetAroundSectors(int32 y, int32 x)
{
	SectorInfo& info = _sectors[y][x];
	info.sector->pos = { y, x };

	for (int d = 0; d < 8; d++)
	{
		Point np = { y - dy[d], x - dx[d] };

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
}