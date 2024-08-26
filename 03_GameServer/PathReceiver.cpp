#include "stdafx.h"
#include "PathReceiver.h"

#include "GameObject.h"
#include "Route.h"

PathReceiver::PathReceiver(GameObject* object)
	: _object(object)
	, _executionCounter(0)
{
	_cur = _route.begin();
}

PathReceiver::~PathReceiver()
{
}

Route& PathReceiver::GetRoute()
{ return _route; }

void PathReceiver::RequestPathFinding(const Position& destination)
{
	_executionCounter++;
	_object->_owner->GetMap()->RequestPathFinding(_object, destination);
}

void PathReceiver::IgnorePreviousPath()
{
	_executionCounter++;
	_route.Destroy();
	ResetCurrentTargetPosition();
}

void PathReceiver::ResetCurrentTargetPosition()
{ _cur = _route.begin(); }

void PathReceiver::SetNextTargetPosition()
{
	if (_route.end() == _cur)
	{
		return;
	}
	++_cur;
}

RouteIterator PathReceiver::CurrentTargetPosition()
{ return _cur; }

RawPacket& operator<<(RawPacket& pkt, const PathReceiver& data)
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


