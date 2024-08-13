#pragma once
enum class TileInfo
{
	MOVABLE,
	OBSTACLE,
};


class MapData
{
public:
	MapData(uint32 width, uint32 height, uint32 widthResolution, uint32 heightResolution);
	vector<TileInfo>& operator[](uint64 idx)
	{
		return _tiles[idx];
	}

private:
	uint32 _width;
	uint32 _height;
	uint32 _widthResolution;
	uint32 _heightResolution;
	vector<vector<TileInfo>> _tiles;
};