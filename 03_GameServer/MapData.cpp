#include "stdafx.h"
#include "MapData.h"

MapData::MapData(const char* fileName)
{
	ifstream f;
	f.open(fileName, ios_base::binary | ios_base::in);

	f.get((char*)&_width, 4).get((char*)&_height, 4);


	_tiles.resize(_height);
	for (int32 i = 0; i < _height; i++)
	{
		_tiles[i].resize(_width);
	}

	for (auto& tileRow : _tiles)
	{
		for (auto& tile : tileRow)
		{
			f.get((char*)&tile, sizeof(TileInfo));
		}
	}

	f.close();
}
