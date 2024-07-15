#include "stdafx.h"
#include "DBWriter.h"

#include "Job.h"
#include "Thread.h"


void DBWriter::dbThread()
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


DBWriter::DBWriter()
{
	_handle = RunThread(&DBWriter::dbThread, this);
}

DBWriter::~DBWriter()
{
	CloseHandle(_handle);
}

DBWriter& DBWriter::Instance()
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

void DBWriter::Write(Job* job)
{
	Instance()._queue.Enqueue(job);
}
