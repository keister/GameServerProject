#pragma once

namespace game
{
	/// @brief 전역의 오브젝트풀을 자동으로 생성하여준다.
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
}
