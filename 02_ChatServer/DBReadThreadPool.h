#pragma once
#include "Job.h"
class Job;
namespace chat
{
	class DBReadThreadPool
	{

	public:
		DBReadThreadPool(int32 numThread)
		{

			for (int32 i = 0; i < numThread; i++)
			{
				_dbReadThread.push_back(thread(&DBReadThreadPool::ThreadFunc, this));
			}
		}

		~DBReadThreadPool()
		{
			_isExit = true;

			for (thread& t : _dbReadThread)
			{
				t.join();
			}
		}

		void ExecuteAsync(Job* job);

	private:
		void ThreadFunc();


	private:
		vector<std::thread>		_dbReadThread;
		std::condition_variable	_cv;
		std::mutex				_jobQueueLock;
		std::queue<Job*>		_jobQueue;

		bool					_isExit;
	};

	
}
