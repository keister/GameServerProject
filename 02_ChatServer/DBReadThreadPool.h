#pragma once


namespace netlib
{
	class Job;
}


namespace chat
{

	/// @brief DB읽기 작업을 비동기로 처리하기 위한 쓰레드풀
	///		   C++의 쓰레드관련 함수를 통해 구현해보기
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
