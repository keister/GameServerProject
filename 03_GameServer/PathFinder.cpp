#include "stdafx.h"
#include "PathFinder.h"

#include "GameObject.h"
#include "MapData.h"
#include "PathReceiver.h"


const vector<game::TilePos> game::PathFinder::UNIT_VECTOR{
		TilePos{  0,  1 },
		TilePos{ -1,  1 },
		TilePos{ -1,  0 },
		TilePos{ -1, -1 },
		TilePos{  0, -1 },
		TilePos{  1, -1 },
		TilePos{  1,  0 },
		TilePos{  1,  1 }
};

pair<game::PathFinder::Direction, game::PathFinder::Direction> game::PathFinder::Split(Direction dir)
{
	if ((int)dir & 2 == 0)
	{
		return make_pair(dir, dir);
	}

	return make_pair((Direction)(((int)dir + 7) % 8), (Direction)(((int)dir + 1) % 8));
}

bool game::PathFinder::IsObstacleBetween(const TilePos& start, const TilePos& end)
{
	TilePos diff = end - start;
	int32 unitX = (diff.x() > 0) ? 1 : (diff.x() < 0) ? -1 : 0;
	int32 unitY = (diff.y() > 0) ? 1 : (diff.y() < 0) ? -1 : 0;

	TilePos unit = { unitX, unitY };


	diff = diff.cwiseAbs();

	TilePos pos = start;
	int32 error = 0;

	if (diff.x() > diff.y())
	{
		for (int32 i = 0; i < diff.x(); i++)
		{
			error += 2 * diff.y();
			if (error > diff.x())
			{
				error -= 2 * diff.x();
				pos.y() += unit.y();
			}
			pos.x() += unit.x();

			if ((*_mapData)[pos] == TileInfo::OBSTACLE)
			{
				return true;
			}
		}
	}
	else
	{
		for (int32 i = 0; i < diff.y(); i++)
		{
			error += 2 * diff.x();
			if (error > diff.y())
			{
				error -= 2 * diff.y();
				pos.x() += unit.x();
			}
			pos.y() += unit.y();

			if ((*_mapData)[pos] == TileInfo::OBSTACLE)
			{
				return true;
			}
		}
	}

	return false;
}

game::Route game::PathFinder::Execute(const Position& start, const Position& destination)
{
	Route ret = Route::empty();

	StartRoutine(start, destination);

	if ((*_mapData)[_start] != TileInfo::OBSTACLE)
	{
		while (!_openNode.empty())
		{
			TileNode* cur = _openNode.back();
			_openNode.pop_back();
			_openNodeMap.erase(cur->pos);
			_closeNode.insert(make_pair(cur->pos, cur));
			if (cur->pos == _destination)
			{
				ret = CreateRoute(cur, destination);
				break;
			}

			Update(cur);
			ranges::sort(_openNode, _comp);
		}
	}

	EndRoutine();

	return ret;
}

bool game::PathFinder::IsInRange(const TilePos& pos)
{
	if (pos.y() < _minHeight || pos.y() >= _maxHeight || pos.x() < _minWidth || pos.x() >= _maxWidth)
	{
		return false;
	}

	return true;
}

void game::PathFinder::Update(TileNode* node)
{
	uint8 curDirection = 1;

	for (int32 i = 0; i < 8; i++, curDirection <<= 1)
	{
		if ((node->validDirFlag & curDirection) != 0)
		{
			Direction dir = (Direction)i;

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

			// 오픈리스트 갱신
			UpdateOpenList(pos, node, dirFlag);
		}
	}
}

bool game::PathFinder::CheckTile(const TilePos& pos, Direction dir)
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

uint8 game::PathFinder::SearchDiagonal(const TilePos& pos, Direction dir, TilePos* nodePoint)
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

		if ((*_mapData)[now] == TileInfo::OBSTACLE)
		{
			return NO_ROUTE;
		}

		if ((*_mapData)[now + UNIT_VECTOR[(int)RotateClockwise<-90>(splitDir.first)]] == TileInfo::OBSTACLE)
		{
			if ((*_mapData)[now + UNIT_VECTOR[(int)RotateClockwise<90>(splitDir.second)]] == TileInfo::OBSTACLE)
			{
				return NO_ROUTE;
			}
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
			*nodePoint = now;
			return flag;
		}

		if (SearchOrthogonal(now, splitDir.first, nodePoint) != NO_ROUTE)
		{
			flag << dir << splitDir.first << splitDir.second;
			*nodePoint = now;
			return flag;
		}

		if (SearchOrthogonal(now, splitDir.second, nodePoint) != NO_ROUTE)
		{
			flag << dir << splitDir.first << splitDir.second;
			*nodePoint = now;
			return flag;
		}

		now += UNIT_VECTOR[(int)dir];
	}
}

uint8 game::PathFinder::SearchOrthogonal(const TilePos& pos, Direction dir, TilePos* nodePoint)
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

void game::PathFinder::UpdateOpenList(const TilePos& pos, TileNode* parent, uint8 flag)
{
	if (_closeNode.contains(pos))
	{
		return;
	}

	float64 g = parent->g + (parent->pos - pos).lpNorm<2>();
	float64 h = (_destination - pos).lpNorm<1>();

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

void game::PathFinder::StartRoutine(const Position& start, const Position& destination)
{
	_destination = { (int)destination.x(), (int)destination.y() };
	_start = { (int)(start.x() * (float32)_mapData->PrecisionWidth()), (int)(start.y() * (float32)_mapData->PrecisionHeight()) };

	_maxWidth = min(_start.x() + 30 * _mapData->PrecisionWidth(), _mapData->Width());
	_maxHeight = min(_start.y() + 30 * _mapData->PrecisionHeight(), _mapData->Height());
	_minWidth = max((int32)(_start.x() - 30 * _mapData->PrecisionWidth()), 0);
	_minHeight = max((int32)(_start.y() - 30 * _mapData->PrecisionHeight()), 0);

	double manhattan = (_destination - _start).lpNorm<1>();
	TileNode* startNode = _tileNodeAllocator.Alloc();
	startNode->parent = nullptr;
	startNode->g = 0;
	startNode->h = manhattan;
	startNode->f = manhattan;
	startNode->pos = _start;
	startNode->validDirFlag = 0xFF;
	_openNode.push_back(startNode);
	_openNodeMap.insert(make_pair(_start, startNode));
}

void game::PathFinder::EndRoutine()
{
	for (TileNode* node : _openNode)
	{
		_tileNodeAllocator.Free(node);
	}
	_openNode.clear();
	_openNodeMap.clear();

	for (auto& it : _closeNode)
	{
		_tileNodeAllocator.Free(it.second);
	}
	_closeNode.clear();
}

/// @brief 브레즌햄 알고리즘을 적용하여, 8방향이루어진 경로를 압축한다.
game::Route game::PathFinder::CreateRoute(TileNode* endNode, const Position& destination)
{
	TileNode* curNode = endNode;
	TileNode* nextTarget = curNode;
	TileNode* nextNode = curNode->parent;

	Route ret;
	ret.push_front(destination);

	if (nextNode == nullptr)
	{
		return ret;
	}

	while (true)
	{
		if (IsObstacleBetween(curNode->pos, nextNode->pos))
		{
			curNode = nextTarget;
			ret.push_front({
				(float32)nextTarget->pos.x() / (_mapData->PrecisionWidth() ) + 0.5f, 
				(float32)nextTarget->pos.y() / _mapData->PrecisionHeight() + 0.5f
			});
		}
		else
		{
			nextTarget = nextNode;
			nextNode = nextNode->parent;
		}

		if (nextTarget->pos == _start)
		{
			break;
		}
	}

	return ret;
}
