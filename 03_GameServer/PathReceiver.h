#pragma once
#include "PathFinder.h"
#include "RawPacket.h"

namespace game
{
	class MapData;
	class GameObject;

	/// @brief ���ӿ�����Ʈ�� ��ã�⸦ ��û�ϰ� �ش� Ŭ������ ���� ��ã�� ����� �޾ƿ´�.
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
		uint64			_executionCounter; ///< �������� ��û�� ������ ��, ���� �ֱ��� �����͸� �ݿ��ϱ� ����
		Route			_route;
		RouteIterator	_cur;
	};

	RawPacket& operator<< (RawPacket& pkt, const game::PathReceiver& data);
}



