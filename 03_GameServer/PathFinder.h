#pragma once

#define NO_ROUTE 0xFF


class GameObject;
class MapData;

using TilePos = Eigen::Vector2<int32>;

template<>
struct std::hash<TilePos>
{
	size_t operator()(const TilePos& pos) const
	{
		return hash<int>()(pos.y()) ^ hash<int>()(pos.x());
	}
};




class PathFinder
{
	enum class Direction
	{
		DD, LD, LL, LU, UU, RU, RR, RD,
	};

	friend uint8& operator<<(uint8& flag, Direction dir);
	;
	static const vector<TilePos> UNIT_VECTOR;
	static const vector<Direction> DIRS;

	using enum Direction;
	static constexpr Direction rotateTable[8] = {DD, LD, LL, LU, UU, RU, RR, RD};

	struct TileNode
	{
		TilePos pos;
		double g;
		double h;
		double f;
		TileNode* parent;
		uint8 validDirFlag;
	};

	struct TileNodeComparator
	{
		bool operator()(TileNode* n1, TileNode* n2)
		{
			return n1->f < n2->f;
		}
	};

	static constexpr TileNodeComparator _comp;

public:
	template <int Degree>
	static Direction RotateClockwise(const Direction& dir)
	{
		static_assert(Degree >= -360 && Degree <= 360 && Degree % 45 == 0);

		if constexpr ( Degree / 45 < 0)
		{
			return static_cast<Direction>(((static_cast<int>(dir) - Degree / 45) + 8) % 8);
		}
		else
		{
			return static_cast<Direction>((static_cast<int>(dir) + Degree / 45) % 8);
		}
	}

	static pair<Direction, Direction> Split(Direction dir)
	{
		if ((int)dir & 2 == 0)
		{
			return make_pair(dir, dir);
		}

		return make_pair((Direction)(((int)dir + 7) % 8), (Direction)(((int)dir + 1) % 8));
	}

	static bool IsDiagonal(Direction dir)
	{
		return (int)dir % 2 == 1;
	}


	PathFinder() {};
	void Execute(Position destination);
	bool IsInRange(const TilePos& pos);
	void Update(TileNode* node);
	bool CheckTile(const TilePos& pos, Direction dir);
	TileNode CreateNode();

	uint8 SearchDiagonal(const TilePos& pos, Direction dir, TilePos* nodePoint);
	uint8 SearchOrthogonal(const TilePos& pos, Direction dir, TilePos* nodePoint);

	void UpdateOpenList(const TilePos& pos, TileNode* parent, uint8 flag);


private:
	MapData* _mapData;
	vector<TileNode*> _openNode;
	unordered_map<TilePos, TileNode*> _openNodeMap;
	unordered_map<TilePos, TileNode*> _closeNode;
	GameObject* _object;

	int32 _maxHeight;
	int32 _maxWidth;
	int32 _minHeight;
	int32 _minWidth;

	TilePos _destination;

	inline static ObjectPoolTls<TileNode, false> _tileNodeAllocator;
};

inline uint8& operator<<(uint8& flag, PathFinder::Direction dir)
{
	flag = flag | (1 << (int)dir);

	return flag;
}