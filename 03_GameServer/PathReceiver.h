#pragma once
#include "PathFinder.h"
#include "RawPacket.h"

namespace game
{
	class MapData;
	class GameObject;

	/// @brief 게임오브젝트가 길찾기를 요청하고 해당 클래스를 통해 길찾기 결과를 받아온다.
	class PathReceiver
	{
		friend RawPacket& operator<< (RawPacket&, const PathReceiver&);

	public:
		PathReceiver(GameObject* object);
		~PathReceiver();

		Route& GetRoute();

		void RequestPathFinding(const Position& destination);
		void IgnorePreviousPath();

		void ResetCurrentTargetPosition();
		void SetNextTargetPosition();

		RouteIterator CurrentTargetPosition();;

		uint64 ExecutionCount()
		{
			return _executionCounter;
		}

	private:
		GameObject*		_object;
		uint64			_executionCounter; ///< 여러번의 요청이 들어왔을 떄, 가장 최근의 데이터만 반영하기 위함
		Route			_route;
		RouteIterator	_cur;
	};

	RawPacket& operator<< (RawPacket& pkt, const game::PathReceiver& data);
}



