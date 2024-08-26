#include "stdafx.h"
#include "Map.h"
#include "GroupBase.h"
#include "PacketHandler.h"
#include "Player.h"
#include "GameObject.h"
#include "MapData.h"

Map::Map(GameGroupBase* group, const char* fileName, int32 sectorWidth, int32 sectorHeight)
	: _mapData(new MapData(fileName))
	, _sectorWidth(sectorWidth)
	, _sectorHeight(sectorHeight)
	, _sectorMaxY(_mapData->Height() / sectorHeight)
	, _sectorMaxX(_mapData->Width() / sectorWidth)
	, _sectors(_sectorMaxY, vector<SectorInfo>(_sectorMaxX))
	, _group(group)
	, _pathFindingWorker(this)
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


const TileInfo& Map::GetTileInfo(const Position& pos)
{
	return (*_mapData)[{(int32)pos.x(), (int32)pos.y()}];
}

Position Map::GetRandomPostionInSection(int32 id)
{
	auto& arr = _mapData->GetMovableTiles(id);

	int32 idx = rand() % arr.size();

	return arr[idx];
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



void Map::SendPacket(Sector* sector, int32 sectorRange, Packet pkt, list<uint64>& except)
{
	int32 currentSector = 1;

	for (int32 idx = 0; idx < 9; idx++, currentSector <<= 1)
	{
		if ((sectorRange & currentSector) != 0)
		{
			Sector* targetSector = _sectors[sector->pos.y][sector->pos.x].aroundSectors[idx];
			if (targetSector == nullptr)
			{
				continue;
			}

			unordered_set<GameHost*>& players = targetSector->hosts;

			for (GameHost* p : players)
			{
				bool isExcept = false;
				for (uint64 id : except)
				{
					if (id == p->SessionId())
					{
						isExcept = true;
						break;
					}
				}

				if (isExcept == false)
				{
					_group->SendPacket(p->SessionId(), pkt);
				}
			}
		}
	}
}

void Map::SendPacket(Sector* sector, int32 sectorRange, Packet pkt, uint64 except)
{
	list list = { except };
	SendPacket(sector, sectorRange, pkt, list);
}

void Map::SendPacket(Sector* sector, int32 sectorRange, Packet pkt)
{
	list<uint64> list = {};
	SendPacket(sector, sectorRange, pkt, list);
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

void Sector::InsertObject(GameObject* obj)
{
	const TypeInfo* info = obj->GetTypeInfo();

	while (true)
	{
		auto it = _classifiedObjects.find(info->id);

		unordered_set<void*>* objects;

		if (it == _classifiedObjects.end())
		{
			objects = &_classifiedObjects.insert({ info->id, {} }).first->second;
		}
		else
		{
			objects = &it->second;
		}

		objects->insert(obj);

		if (info->super == nullptr)
		{
			break;
		}

		info = info->super;
	}

}

void Sector::EraseObject(GameObject* obj)
{
	const TypeInfo* info = obj->GetTypeInfo();

	while (true)
	{
		auto it = _classifiedObjects.find(info->id);

		unordered_set<void*>* objects = &it->second;

		objects->erase(obj);

		if (info->super == nullptr)
		{
			break;
		}

		info = info->super;
	}
}
