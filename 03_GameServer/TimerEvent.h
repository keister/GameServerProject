#pragma once
#include <functional>

struct TimerEvent
{
	DWORD reservedTick;
	function<void()> func;

	void operator() () const
	{
		func();
	}

	bool operator< (const TimerEvent& other) const
	{
		return reservedTick > other.reservedTick;
	}
};
