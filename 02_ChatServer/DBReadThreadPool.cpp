#include "stdafx.h"
#include "DBReadThreadPool.h"
#include "Job.h"

void DBReadThreadPool::ExecuteAsync(Job* job)
{
	{
		std::lock_guard guard(_jobQueueLock);
		_jobQueue.push(job);
	}

	_cv.notify_one();
}

void DBReadThreadPool::ThreadFunc()
{

	while (!_isExit)
	{
		std::unique_lock uniqueLock(_jobQueueLock);

		_cv.wait(
			uniqueLock, [&] {return !_jobQueue.empty(); }
		);

		while (!_jobQueue.empty())
		{
			Job* job = _jobQueue.front();
			job->Execute();
			Job::Free(job);
			_jobQueue.pop();
		}


		uniqueLock.unlock();
	}

}
