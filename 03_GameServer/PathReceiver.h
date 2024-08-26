#pragma once
#include "PathFinder.h"
#include "RawPacket.h"

class MapData;
class GameObject;

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
	uint64			_executionCounter;
	Route			_route;
	RouteIterator	_cur;
};


RawPacket& operator<< (RawPacket& pkt, const PathReceiver& data);


