#pragma once
#include "JobQueue.h"


class Job;

class DBWriter
{

public:
	DBWriter();
	~DBWriter();
	static DBWriter& Instance();

	static void Write(Job* job);
private:
	void dbThread();
	inline static Lock _lock;
	JobQueue _queue;
	HANDLE _handle;
};

