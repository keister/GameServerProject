#pragma once
#include "ObjectPoolTls.h"


namespace netlib
{
	enum
	{
		JOB_MAX_SIZE = 256,
	};

	struct JobBlock
	{
		BYTE Data[JOB_MAX_SIZE];
	};


	template <typename Func> requires std::is_invocable_v<Func>
	class CallableJob;

	class Job
	{
		friend ObjectPoolTls<JobBlock, false>;

	protected:
		Job() {};
	public:
		virtual ~Job() = default;
		virtual void Execute() = 0;

		template<typename T, typename... Args> requires is_base_of_v<Job, T>
		static T* Alloc(Args&&... args)
		{
			static_assert(sizeof(T) < JOB_MAX_SIZE);

			void* ptr = _jobPool.Alloc();

			T* job = new(ptr)T(std::forward<Args>(args)...);
			return job;
		}

		template<typename Func> requires std::is_invocable_v<Func>
		static CallableJob<Func>* Alloc(Func func)
		{
			static_assert(sizeof(CallableJob<Func>) < JOB_MAX_SIZE);

			void* ptr = _jobPool.Alloc();

			CallableJob<Func>* job = new(ptr)CallableJob<Func>(func);
			return job;
		}

		static void Free(Job* job)
		{

			job->~Job();

			_jobPool.Free((JobBlock*)job);

		}

		static int32 GetUseCount()
		{
			return _jobPool.GetUseCount();
		}

		static int32 GetCapacity()
		{
			return _jobPool.GetCapacity();
		}


	private:
		inline static ObjectPoolTls<JobBlock, false> _jobPool;
	};


	template <typename Func> requires std::is_invocable_v<Func>
	class CallableJob : public Job
	{
	public:
		CallableJob(Func func)
			: _func(func) {}

		void Execute() override
		{
			_func();
		}

		Func _func;
	};
}
