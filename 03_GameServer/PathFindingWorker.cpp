#include "stdafx.h"
#include "PathFindingWorker.h"

#include "GameObject.h"
#include "Job.h"
#include "PathReceiver.h"
#include "Route.h"

PathFindingWorker::PathFindingWorker(Map* map)
	: _map(map)
	, _pathFinder(map->GetData())
	, _isExit(false)
	, _thread(&PathFindingWorker::ThreadFunc, this)
{
}

void PathFindingWorker::ThreadFunc()
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

void PathFindingWorker::RequestPathFinding(GameObject* gameObject, const Position& destination)
{


	uint64 objectId = gameObject->ObjectId();
	uint64 executionCount = gameObject->GetPathReciever().ExecutionCount();


	Job* job = Job::Alloc(
		[=, this]
		{
			ExecutePathFinding(gameObject, objectId, destination, executionCount);
		}
	);

	{
		std::lock_guard guard(_jobQueueLock);
		_jobQueue.push(job);
	}

	_cv.notify_one();
}

void PathFindingWorker::ExecutePathFinding(GameObject* gameObject, uint64 objectId, const Position& destination,
	uint64 execCount)
{

	Position start = gameObject->position;

	Route ret = _pathFinder.Execute(start, destination);

	_totalPathFindingCnt++;

	if (ret.IsEmpty())
	{
		return;
	}

	Job* job = Job::Alloc([=, this]
		{
			Route r = ret;
			_map->_group->PathFindingCompletionRoutine(gameObject, objectId, execCount, r);
		});

	_map->_group->EnqueueJob(job);
	
}
