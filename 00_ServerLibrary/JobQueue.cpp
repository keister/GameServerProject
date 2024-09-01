#include "stdafx.h"
#include "JobQueue.h"
#include "Profiler.h"

netlib::JobQueue::JobQueue()
	: _event(CreateEvent(nullptr, false, false, nullptr))
{
}

netlib::JobQueue::~JobQueue()
{
	CloseHandle(_event);
}

void netlib::JobQueue::Enqueue(Job* job)
{
	SCOPE_PROFILE("Job Enqueue");
	_queue.Push(job);
	SetEvent(_event);
}

bool netlib::JobQueue::Dequeue(Job*& job)
{
	SCOPE_PROFILE("Job Dequeue");
	return _queue.Pop(job);
}

void netlib::JobQueue::Wait()
{
	WaitForSingleObject(_event, INFINITE);
}
