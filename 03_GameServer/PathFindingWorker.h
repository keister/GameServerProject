#pragma once
#include "Job.h"
#include "PathFinder.h"

namespace netlib
{
	class Job;
}

namespace game
{
	class Map;
	class PathReceiver;

	/// @brief JPS 비동기 쓰레드. 하나의 맵에 하나씩 붙는다.
	class PathFindingWorker
	{
	public:
		PathFindingWorker(Map* map);

		void ThreadFunc();
		void RequestPathFinding(GameObject* gameObject, const Position& destination);
		uint64 GetTotalPathFindingCount()
		{
			return _totalPathFindingCnt;
		}

	private:
		void ExecutePathFinding(GameObject* gameObject, uint64 objectId, const Position& destination, uint64 execCount);

	private:
		Map* _map;
		std::condition_variable _cv;
		std::mutex				_jobQueueLock;
		std::queue<Job*>		_jobQueue;
		PathFinder				_pathFinder;
		bool					_isExit;
		std::thread				_thread;
		uint64					_totalPathFindingCnt;
	};
}
