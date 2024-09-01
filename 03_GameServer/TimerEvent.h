#pragma once
#include <functional>

namespace game
{

	class BaseObject;
	class GameObject;

	/// @brief ���� ƽ�̳� �ð��ڿ� �Ͼ �̺�Ʈ�� ������ �� ���� ����ü
	struct TimerEvent
	{
		DWORD				reservedTick;	///< ���� �ڿ� �Ͼ ������
		function<void()>	func;			///< ����� �Լ�
		BaseObject*			gameObject;		///< �Լ� ����ÿ� �ش� ������Ʈ�� �ı��Ǿ����� Ȯ���ϱ� ����
		uint64				objectId;		///< �Լ� ����ÿ� �ش� ������Ʈ�� �ı��Ǿ����� Ȯ���ϱ� ����

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
