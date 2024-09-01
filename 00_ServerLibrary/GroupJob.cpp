#include "stdafx.h"
#include "GroupJob.h"

#include "GroupBase.h"
#include "RawPacket.h"



void netlib::GroupJob_EnterSession::Execute()
{

	SessionManager& sessionManager = _group->_server->_sessionManager;
	Session* session = sessionManager.Acquire(_sessionId);

	if (session == nullptr)
	{
		return;
	}
	_group->_sessionList.insert(_sessionId);
	sessionManager.Release(*session);

	_group->OnEnter(_sessionId);
}

void netlib::GroupJob_LeaveSession::Execute()
{
	int64 ret = _group->_sessionList.erase(_sessionId);
	if (ret != 0)
	{
		_group->OnLeave(_sessionId);
		if (_disconnect == true)
		{
			_group->_server->OnDisconnect(_sessionId);
		}
	}
}
