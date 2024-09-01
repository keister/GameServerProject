#include "stdafx.h"
#include "PathReceiver.h"

#include "GameObject.h"
#include "Route.h"

game::PathReceiver::PathReceiver(GameObject* object)
	: _object(object)
	, _executionCounter(0)
{
	_cur = _route.begin();
}

game::PathReceiver::~PathReceiver()
{
}

game::Route& game::PathReceiver::GetRoute()
{ return _route; }

void game::PathReceiver::RequestPathFinding(const Position& destination)
{
	_executionCounter++;
	_object->_owner->GetMap()->RequestPathFinding(_object, destination);
}

void game::PathReceiver::IgnorePreviousPath()
{
	_executionCounter++;
	_route.Destroy();
	ResetCurrentTargetPosition();
}

void game::PathReceiver::ResetCurrentTargetPosition()
{ _cur = _route.begin(); }

void game::PathReceiver::SetNextTargetPosition()
{
	if (_route.end() == _cur)
	{
		return;
	}
	++_cur;
}

game::RouteIterator game::PathReceiver::CurrentTargetPosition()
{ return _cur; }

RawPacket& game::operator<<(RawPacket& pkt, const PathReceiver& data)
{
	list<RouteIterator> l;

	for (RouteIterator it = data.PathReceiver::_cur; it != data.PathReceiver::_route.end(); ++it)
	{
		l.push_back(it);
	}

	pkt << (uint16)l.size();

	for (auto& it : l)
	{
		pkt << it->y() << it->x();
	}

	return pkt;
}

