#include "stdafx.h"
#include "DBWriter.h"

#include "Job.h"
#include "Thread.h"


void game::DBWriter::dbwrite_thread_func()
{
	while (true)
	{
		Job* job;
		_queue.Wait();
		while (_queue.Size() > 0)
		{
			_queue.Dequeue(job);
			job->Execute();
		}
	}
}


game::DBWriter::DBWriter()
{
	_handle = RunThread(&DBWriter::dbwrite_thread_func, this);
}

game::DBWriter::~DBWriter()
{
	CloseHandle(_handle);
}

game::DBWriter& game::DBWriter::Instance()
{
	static DBWriter* instance = nullptr;
	if (instance == nullptr)
	{
		WRITE_LOCK(_lock);
		if (instance == nullptr)
		{
			instance = new DBWriter();
		}
	}

	return *instance;
}

void game::DBWriter::Write(Job* job)
{
	Instance()._queue.Enqueue(job);
}
