#pragma once
enum class TileInfo : uint8
{
	MOVABLE,
	OBSTACLE,
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


	MapData(const char* fileName);
	vector<TileInfo>& operator[](uint64 idx)
	{
		return _tiles[idx];
	}

	TileInfo& operator[](const Eigen::Vector2<int32>& vec)
	{
		return _tiles[vec.y()][vec.x()];
	}

private:
	uint32 _width;
	uint32 _height;
	vector<vector<TileInfo>> _tiles;
};