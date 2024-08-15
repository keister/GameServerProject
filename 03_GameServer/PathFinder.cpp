#include "stdafx.h"
#include "PathFinder.h"

#include "GameObject.h"
#include "MapData.h"



const vector<TilePos> PathFinder::UNIT_VECTOR{
		TilePos{  1,  0 },
		TilePos{  1, -1 },
		TilePos{  0, -1 },
		TilePos{ -1, -1 },
		TilePos{ -1,  0 },
		TilePos{ -1,  1 },
		TilePos{  0,  1 },
		TilePos{  1,  1 }
};


const vector<PathFinder::Direction> PathFinder::DIRS{
		DD, LD, LL, LU, UU, RU, RR, RD
};

void PathFinder::Execute(Position destination)
{
	_openNode.clear();
	_openNodeMap.clear();
	_closeNode.clear();
	_destination = { (int)destination.x(), (int)destination.y()};
	TilePos startPos = { (int)_object->position.x(), (int)_object->position.y() };

	double manhattan = (_destination - startPos).lpNorm<1>();
	TileNode* startNode = _tileNodeAllocator.Alloc();
	startNode->parent = nullptr;
	startNode->g = 0;
	startNode->h = manhattan;
	startNode->f = manhattan;
	startNode->pos = startPos;
	startNode->validDirFlag = 0xFF;

	while (!_openNode.empty())
	{
		TileNode* cur = _openNode.back();
		_openNode.pop_back();
		_openNodeMap.erase(cur->pos);
		_closeNode.insert(make_pair(cur->pos, cur));
		if (cur->pos == _destination)
		{
			// 탐색끝
		}

		Update(cur);
		ranges::sort(_openNode, _comp);
	}

}

bool PathFinder::IsInRange(const TilePos& pos)
{
	if (pos.y() < _minHeight || pos.y() >= _maxHeight || pos.x() < _minWidth || pos.x() >= _maxWidth)
	{
		return false;
	}

	return true;
}

void PathFinder::Update(TileNode* node)
{
	uint8 curDirection = 1;

	for (int32 i = 0; i < 8; i++, curDirection <<= 1)
	{
		if ((node->validDirFlag & curDirection) != 0)
		{
			Direction dir = DIRS[i];

			uint8 dirFlag = 0;
			TilePos pos;

			if (IsDiagonal(dir))
			{
				dirFlag = SearchDiagonal(node->pos, dir, &pos);
			}
			else
			{
				dirFlag = SearchOrthogonal(node->pos, dir, &pos);
			}

			if (dirFlag == NO_ROUTE)
			{
				continue;
			}

			UpdateOpenList(pos, node, dirFlag);
		}
	}
}

bool PathFinder::CheckTile(const TilePos& pos, Direction dir)
{
	TilePos next = pos + UNIT_VECTOR[(int)dir];

	if (!(IsInRange(pos) && IsInRange(next)))
	{
		return false;
	}

	if ((*_mapData)[pos.y()][pos.x()] == TileInfo::OBSTACLE &&
		(*_mapData)[next.y()][next.x()] != TileInfo::OBSTACLE)
	{
		return true;
	}

	return false;
}

uint8 PathFinder::SearchDiagonal(const TilePos& pos, Direction dir, TilePos* nodePoint)
{
	TilePos now{ pos + UNIT_VECTOR[(int)dir] };

	uint8 flag = 0;

	while (true)
	{
		if (!IsInRange(now))
		{
			return NO_ROUTE;
		}
		auto splitDir = Split(dir);

		// TODO: 대각선 범위체크
		if ((*_mapData)[now] == TileInfo::OBSTACLE)
		{
			return NO_ROUTE;
		}

		if ((*_mapData)[now + UNIT_VECTOR[(int)RotateClockwise<-90>(splitDir.first)]] == TileInfo::OBSTACLE)
		{
			return NO_ROUTE;
		}

		if ((*_mapData)[now + UNIT_VECTOR[(int)RotateClockwise<90>(splitDir.second)]] == TileInfo::OBSTACLE)
		{
			return NO_ROUTE;
		}

		if (now == _destination)
		{
			*nodePoint = now;
			return flag;
		}



		if (CheckTile(now + UNIT_VECTOR[(int)RotateClockwise<-90>(splitDir.first)], splitDir.first))
		{
			flag << RotateClockwise<-90>(dir);
		}

		if (CheckTile(now + UNIT_VECTOR[(int)RotateClockwise<90>(splitDir.second)], splitDir.second))
		{
			flag << RotateClockwise<90>(dir);
		}

		if (flag != 0)
		{
			flag << dir << splitDir.first << splitDir.second;
			return flag;
		}

		if (SearchOrthogonal(now + UNIT_VECTOR[(int)splitDir.first], splitDir.first, nodePoint) != NO_ROUTE)
		{
			flag << dir << splitDir.first << splitDir.second;
			*nodePoint = now;
			return flag;
		}

		if (SearchOrthogonal(now + UNIT_VECTOR[(int)splitDir.second], splitDir.second, nodePoint) != NO_ROUTE)
		{
			flag << dir << splitDir.first << splitDir.second;
			*nodePoint = now;
			return flag;
		}

		now += UNIT_VECTOR[(int)dir];
	}
}

uint8 PathFinder::SearchOrthogonal(const TilePos& pos, Direction dir, TilePos* nodePoint)
{
	TilePos now{ pos + UNIT_VECTOR[(int)dir] };

	uint8 flag = 0;

	while (true)
	{
		if (!IsInRange(now))
		{
			return NO_ROUTE;
		}

		if ((*_mapData)[now] == TileInfo::OBSTACLE)
		{
			return NO_ROUTE;
		}

		if (now == _destination)
		{
			*nodePoint = now;
			return flag;
		}

		if (CheckTile(now + UNIT_VECTOR[(int)RotateClockwise<-90>(dir)], dir))
		{
			if ((*_mapData)[now + UNIT_VECTOR[(int)dir]] != TileInfo::OBSTACLE)
			{
				flag << RotateClockwise<-45>(dir);
			}
		}

		if (CheckTile(now + UNIT_VECTOR[(int)RotateClockwise<90>(dir)], dir))
		{
			if ((*_mapData)[now + UNIT_VECTOR[(int)dir]] != TileInfo::OBSTACLE)
			{
				flag << RotateClockwise<45>(dir);
			}
		}


		if (flag != 0)
		{
			flag << dir;
			*nodePoint = now;
			return flag;
		}

		now += UNIT_VECTOR[(int)dir];
	}
}

void PathFinder::UpdateOpenList(const TilePos& pos, TileNode* parent, uint8 flag)
{
	if (_closeNode.contains(pos))
	{
		return;
	}

	double g = parent->g + (parent->pos - pos).lpNorm<2>();
	double h = (_destination - pos).lpNorm<1>();

	auto it = _openNodeMap.find(pos);
	if (it != _openNodeMap.end())
	{
		TileNode* node = it->second;
		if (node->g > g)
		{
			node->parent = parent;
			node->g = g;
			node->f = g + h;
		}
	}
	else
	{
		TileNode* node = _tileNodeAllocator.Alloc();
		node->parent = parent;
		node->g = g;
		node->h = h;
		node->f = g + h;
		node->pos = pos;
		node->validDirFlag = flag;

		_openNode.push_back(node);
		_openNodeMap.insert(make_pair(node->pos, node));
	}
}

