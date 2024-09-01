#pragma once

#define NO_ROUTE 0xFF
#include "Route.h"


namespace game
{
	using TilePos = Eigen::Vector2<int32>;
}

template<>
struct std::hash<game::TilePos>
{
	size_t operator()(const game::TilePos& pos) const
	{
		return hash<int>()(pos.y()) ^ hash<int>()(pos.x());
	}
};

namespace game
{
	class GameObject;
	class MapData;
	struct RouteNode;

	/// @brief JPS 알고리즘을 사용하 길찾기 클래스
	class PathFinder
	{
		enum class Direction
		{
			DD, LD, LL, LU, UU, RU, RR, RD,
		};

		friend uint8& operator<<(uint8& flag, Direction dir);
		;
		static const vector<TilePos> UNIT_VECTOR;

		struct TileNode
		{
			TilePos		pos;
			float64		g;
			float64		h;
			float64		f;
			TileNode* parent;
			uint8		validDirFlag;
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


	public:
		template <int Degree>
		static Direction RotateClockwise(const Direction& dir);
		static pair<Direction, Direction> Split(Direction dir);
		static bool IsDiagonal(Direction dir) { return (int)dir % 2 == 1; }
		bool IsObstacleBetween(const TilePos& start, const TilePos& end);

		PathFinder(MapData* map) : _mapData(map) {}
		Route Execute(const Position& start, const Position& destination);
		bool IsInRange(const TilePos& pos);
		void Update(TileNode* node);
		bool CheckTile(const TilePos& pos, Direction dir);

		uint8 SearchDiagonal(const TilePos& pos, Direction dir, TilePos* nodePoint);
		uint8 SearchOrthogonal(const TilePos& pos, Direction dir, TilePos* nodePoint);

		void UpdateOpenList(const TilePos& pos, TileNode* parent, uint8 flag);
	private:
		void StartRoutine(const Position& start, const Position& dest);
		void EndRoutine();

		Route CreateRoute(TileNode* endNode, const Position& destination);

	private:
		MapData* _mapData;
		vector<TileNode*> _openNode;
		unordered_map<TilePos, TileNode*> _openNodeMap;
		unordered_map<TilePos, TileNode*> _closeNode;

		int32 _maxHeight;
		int32 _maxWidth;
		int32 _minHeight;
		int32 _minWidth;

		TilePos _destination;
		TilePos _start;

		inline static ObjectPoolTls<TileNode, false> _tileNodeAllocator;

	};

	/// @brief 현재 방향을 시계방향으로 돌린다.
	/// @tparam Degree 45도 단위로만 가능함(8방향)
	/// @param dir 돌릴 기준 방향
	/// @return 돌린 방향
	template <int Degree>
	PathFinder::Direction PathFinder::RotateClockwise(const Direction& dir)
	{
		static_assert(Degree >= -360 && Degree <= 360 && Degree % 45 == 0);

		if constexpr (Degree / 45 < 0)
		{
			return static_cast<Direction>(((static_cast<int>(dir) + Degree / 45) + 8) % 8);
		}
		else
		{
			return static_cast<Direction>((static_cast<int>(dir) + Degree / 45) % 8);
		}
	}

	inline uint8& operator<<(uint8& flag, PathFinder::Direction dir)
	{
		flag = flag | (uint8)(1 << (uint8)dir);

		return flag;
	}
}
