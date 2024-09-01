#pragma once
#include "Job.h"

namespace netlib
{
	class RawPacket;
	class GroupBase;

	class GroupJob_EnterSession : public Job
	{
	public:
		GroupJob_EnterSession(GroupBase* group, uint64 sessionId)
			: _group(group),
			_sessionId(sessionId)
		{
		}
		void Execute() override;

	private:
		GroupBase* _group;
		uint64	   _sessionId;
	};


	class GroupJob_LeaveSession : public Job
	{
	public:
		GroupJob_LeaveSession(GroupBase* group, uint64 sessionId, bool disconnect)
			: _group(group),
			_sessionId(sessionId),
			_disconnect(disconnect)
		{
		}

		void Execute() override;

	private:
		GroupBase* _group;
		uint64	   _sessionId;
		bool _disconnect;
	};
}
