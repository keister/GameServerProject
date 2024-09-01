#pragma once
#include <functional>

namespace game
{

	class BaseObject;
	class GameObject;

	/// @brief 일정 틱이나 시간뒤에 일어날 이벤트를 예약할 때 쓰는 구조체
	struct TimerEvent
	{
		DWORD				reservedTick;	///< 몇초 뒤에 일어날 것인지
		function<void()>	func;			///< 실행될 함수
		BaseObject*			gameObject;		///< 함수 실행시에 해당 오브젝트가 파괴되었는지 확인하기 위함
		uint64				objectId;		///< 함수 실행시에 해당 오브젝트가 파괴되었는지 확인하기 위함

		void operator() () const
		{
			func();
		}

		bool operator< (const TimerEvent& other) const
		{
			return reservedTick > other.reservedTick;
		}
	};
}
