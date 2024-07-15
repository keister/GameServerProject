#pragma once
#include "Packet.h"

class GroupBase;
class Player;

enum SectorRange
{
	UP					= 0x01,
	LEFT_UP				= 0x02,
	LEFT				= 0x04,
	LEFT_DOWN			= 0x08,
	DOWN				= 0x10,
	RIGHT_DOWN			= 0x20,
	RIGHT				= 0x40,
	RIGHT_UP			= 0x80,
	CENTER				= 0x100,

	COL_CENTER			= UP | CENTER | DOWN,
	COL_RIGHT			= RIGHT_UP | RIGHT | RIGHT_DOWN,
	COL_LEFT			= LEFT_UP | LEFT | LEFT_DOWN,

	ROW_UP				= LEFT_UP | UP | RIGHT_UP,
	ROW_CENTER			= LEFT_DOWN | DOWN | RIGHT_DOWN,
	ROW_DOWN			= LEFT | CENTER | RIGHT,

	COL_LEFT_CENTER		= COL_LEFT | COL_CENTER,
	COL_RIGHT_CENTER	= COL_RIGHT | COL_CENTER,

	ROW_UP_CENTER		= ROW_UP | ROW_CENTER,
	ROW_DOWN_CENTER		= ROW_DOWN | ROW_CENTER,

	ARROW_LEFT_UP		= COL_LEFT | ROW_UP,
	ARROW_RIGHT_UP		= COL_RIGHT | ROW_UP,
	ARROW_LEFT_DOWN		= COL_LEFT | ROW_DOWN,
	ARROW_RIGHT_DOWN	= COL_RIGHT | ROW_DOWN,

	AROUND				= 0x1FF,
};

class Sector
{
public:
	Point pos;
	unordered_set<Player*> players;
};


class Map
{

public:
	static constexpr int32 dy[] = { 1, 1, 0, -1, -1, -1, 0, 1};
	static constexpr int32 dx[] = { 0, -1, -1, -1, 0, 1, 1, 1};

	struct SectorInfo
	{
		Sector* sector;
		vector<Sector*> aroundSectors{9, nullptr};
	};

	Map(GroupBase* group, int32 width, int32 height, int32 sectorWidth, int32 sectorHeight);

	void SendPacket(Player& player, int32 sectorRange, Packet& pkt, bool sendToOwn);
	Sector* GetSector(int32 y, int32 x)
	{
		return _sectors[y][x].sector;
	}

	Sector* FindSectorByPosition(int32 y, int32 x)
	{
		int32 sectorY = y / _sectorHeight;
		int32 sectorX = x / _sectorWidth;

		return _sectors[sectorY][sectorX].sector;
	}

	const vector<Sector*> GetAroundSectors(Sector* sector)
	{
		return _sectors[sector->pos.y][sector->pos.x].aroundSectors;
	}

private:
	void SetAroundSectors(int32 y, int32 x);

private:
	int32 _width;
	int32 _height;
	int32 _sectorWidth;
	int32 _sectorHeight;
	int32 _sectorMaxY;
	int32 _sectorMaxX;
	vector<vector<SectorInfo>> _sectors;
	GroupBase* _group;
};

