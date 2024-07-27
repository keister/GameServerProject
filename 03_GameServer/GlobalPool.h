#pragma once


template <typename T>
class GlobalPool
{
public:
	template<typename ...Args>
	static T* Alloc(Args... args)
	{
		return _pool.Alloc(args...);

	}


	static void Free(void* ptr)
	{
		_pool.Free((T*)ptr);
	}

private:
	inline static ObjectPoolTls<T> _pool;
};

