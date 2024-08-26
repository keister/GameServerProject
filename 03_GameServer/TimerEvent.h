#pragma once
#include <functional>

class BaseObject;
class GameObject;

struct TimerEvent
{
	DWORD reservedTick;
	function<void()> func;
	BaseObject* gameObject;
	uint64		objectId;

	void operator() () const
	{
		func();
	}

	bool operator< (const TimerEvent& other) const
	{
		return reservedTick > other.reservedTick;
	}
};
