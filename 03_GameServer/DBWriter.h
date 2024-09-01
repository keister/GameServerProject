#pragma once
#include "JobQueue.h"

namespace netlib
{
	class Job;
}

namespace game
{
	class DBWriter
	{
	public:
		inline static Lock _lock;
		static DBWriter& Instance();
		static void Write(Job* job);

	public:
		DBWriter();
		~DBWriter();

	private:
		void		dbwrite_thread_func();

	private:
		JobQueue	_queue;
		HANDLE		_handle;
	};
}
