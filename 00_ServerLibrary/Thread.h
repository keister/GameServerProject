#pragma once
#include <functional>


namespace netlib
{
	template <typename T>
	HANDLE RunThread(void(T::* memFunc)(), T* owner);
	template <typename T>
	unsigned WINAPI FunctionWrapper(void* param);

	template <typename T>
	struct ThreadMemFunc
	{
		void(T::* memFunc)();
		T* owner;
	};

	template <typename T>
	HANDLE RunThread(void(T::* memFunc)(), T* owner)
	{
		ThreadMemFunc<T>* func = new ThreadMemFunc<T>(memFunc, owner);

		HANDLE ret = reinterpret_cast<HANDLE>(_beginthreadex(nullptr, NULL, FunctionWrapper<T>, func, NULL, nullptr));


		return ret;
	}


	template <typename T>
	unsigned WINAPI FunctionWrapper(void* param)
	{
		ThreadMemFunc<T>* info = static_cast<ThreadMemFunc<T>*>(param);
		T* caller = info->owner;
		void(T:: * memFunc)() = info->memFunc;

		(caller->*memFunc)();

		delete info;

		return 0;
	}
}
