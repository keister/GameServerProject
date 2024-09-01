#include "stdafx.h"

#include "GroupBase.h"
#include "GroupJob.h"
#include "Job.h"
#include "Thread.h"
#include "JobQueue.h"
#include "RawPacket.h"
#include "Session.h"
#include "Profiler.h"

netlib::GroupBase::GroupBase()
	: _jobQueue(new JobQueue)
	, _evThreadStart(CreateEvent(nullptr, false, false, nullptr))
{
	_threadHandle = RunThread(&GroupBase::thread_func, this);
}

netlib::GroupBase::~GroupBase()
{
}

void netlib::GroupBase::EnqueueJob(Job* job)
{
	_jobQueue->Enqueue(job);
}

void netlib::GroupBase::MoveGroup(uint64 sessionId, int8 dest)
{
	Session* session = _server->_sessionManager.Acquire(sessionId);

	if (session == nullptr)
	{
		return;
	}

	bool result = reserve_erase(sessionId);
	if (result == true)
	{
		OnLeave(sessionId);
	}

	InterlockedExchange8(&session->groupId, dest);
	GroupBase* destGroup = _server->_groups[dest];
	destGroup->EnqueueJob(Job::Alloc<GroupJob_EnterSession>(destGroup, sessionId));


	_server->_sessionManager.Release(*session);
}

void netlib::GroupBase::thread_func()
{
	WaitForSingleObject(_evThreadStart, INFINITE);
	DWORD sleepTime = 0;
	if (_fps != 0)
	{
		sleepTime = 1000 / _fps;
	}

	DWORD idealTime = timeGetTime();
	DWORD overTime = 0;

	DWORD prevTick = timeGetTime();
	while (true)
	{
		process_job();
		process_packet();

		_currentTick = timeGetTime();
		_deltaTick = _currentTick - prevTick;
		prevTick = _currentTick;
		UpdateFrame();

		for (uint64 sessionId : _eraseList)
		{
			_sessionList.erase(sessionId);
		}

		_eraseList.clear();

		_frameCount++;
		if (sleepTime == 0)
		{
			continue;
		}

		//LOG_DBG(L"Group", L"tid : %d, sleepTime : %d", GetCurrentThreadId(), sleepTime);


		::Sleep(sleepTime - overTime);
		idealTime += sleepTime;
		overTime = timeGetTime() - idealTime;
		if (overTime >= sleepTime)
		{
			overTime = sleepTime;
		}
	}
}


void netlib::GroupBase::process_job()
{
	int64 size = _jobQueue->Size();
	Job* job;
	while (size > 0)
	{
		_jobQueue->Dequeue(job);
		job->Execute();
		Job::Free(job);
		size--;
	}
}

void netlib::GroupBase::process_packet()
{
	RawPacket* pkt;

	for (uint64 sessionId : _sessionList)
	{

		Session* session = _server->_sessionManager.Acquire(sessionId);

		if (session == nullptr)
		{
			continue;
		}

		int32 size = session->messegeQueue.Size();

		while (size > 0)
		{
			if (session->groupId != _groupIndex)
			{
				break;
			}

			session->messegeQueue.Pop(pkt);

			OnRecv(sessionId, pkt);
			pkt->DecRef();
			size--;
		}

		if(session->sendBuffer.Size() != 0)
		{
			_server->_iocp.Post(0xFFFFFFFF, session->id, nullptr);
		}

		_server->_sessionManager.Release(*session);
	}
}

