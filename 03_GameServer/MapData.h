#pragma once
enum class TileInfo : uint8
{
	MOVABLE,
	OBSTACLE,
	MONSTER_SECTION,
};

struct MonsterSpawnData
{
	uint64 id;
	float32 x;
	float32 y;
	int32 sectionId;
};

struct SectionInfo
{
	int32 id;
	vector<Position> canMoveTile;
};

class MapData
{
public:
	uint32 Width() const
	{
		return _width;
	}

	uint32 Height() const
	{
		return _height;
	}

	uint32 PrecisionWidth() const
	{
		return _precisionWidth;
	}

	uint32 PrecisionHeight() const
	{
		return _precisionHeight;
	}

	MapData(const char* fileName);
	vector<TileInfo>& operator[](uint64 idx)
	{
		return _tiles[idx];
	}
	TileInfo& operator[](const Eigen::Vector2<int32>& vec)
	{
		return _tiles[vec.y()][vec.x()];
	}
	vector<MonsterSpawnData>& GetMonsterSpawnPoint()
	{
		return _monsterSpawn;
	}

	vector<Position>& GetMovableTiles(int32 id)
	{
		return _section[id].canMoveTile;
	}

private:
	uint32 _width;
	uint32 _height;
	uint32 _precisionWidth;
	uint32 _precisionHeight;
	vector<vector<TileInfo>> _tiles;
	vector<MonsterSpawnData> _monsterSpawn;
	vector<SectionInfo> _section;
};