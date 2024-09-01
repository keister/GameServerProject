#pragma once

namespace netlib
{
	class Job;

	class JobQueue
	{
	public:
		JobQueue();
		~JobQueue();

		void Enqueue(Job* job);
		bool Dequeue(Job*& job);
		void Wait();

		int64 Size() { return _queue.Size(); }

	private:
		LockFreeQueue<Job*>	_queue;
		HANDLE					_event;
	};
}
