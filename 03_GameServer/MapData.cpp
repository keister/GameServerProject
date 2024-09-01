#include "stdafx.h"
#include "MapData.h"

game::MapData::MapData(const char* fileName)
{
	ifstream f;
	f.open(fileName, ios_base::binary | ios_base::in);

	f.read((char*)&_width, 4);
	f.read((char*)&_height, 4);
	f.read((char*)&_precisionWidth, 4);
	f.read((char*)&_precisionHeight, 4);

	_tiles.resize(_height);
	for (int32 i = 0; i < _height; i++)
	{
		_tiles[i].resize(_width);
	}

	for (auto& tileRow : _tiles)
	{
		for (auto& tile : tileRow)
		{
			f.read((char*)&tile, sizeof(TileInfo));
		}
	}

	int32 sectionCount;
	f.read((char*)&sectionCount, sizeof(int32));

	for (int32 i = 0; i < sectionCount; i++)
	{
		_section.push_back({});
		f.read((char*)&_section[i].id, sizeof(int32));

		int32 posCount;
		f.read((char*)&posCount, sizeof(int32));

		for (int32 j = 0; j < posCount; j++)
		{
			float32 x, y;
			f.read((char*)&x, sizeof(float32));
			f.read((char*)&y, sizeof(float32));
			_section[i].canMoveTile.push_back({ x, y });
		}

		int32 monsterCount;
		f.read((char*)&monsterCount, sizeof(int32));

		for (int32 j = 0; j < monsterCount; j++)
		{
			uint64 id;
			float32 x;
			float32 y;
			f.read((char*)&id, sizeof(uint64));
			f.read((char*)&x, sizeof(float32));
			f.read((char*)&y, sizeof(float32));


			_monsterSpawn.push_back({ id, x, y, _section[i].id });
		}

	}


	f.close();
}
