#include "stdafx.h"
#include "SessionManager.h"

#include "HeteroServerBase.h"
#include "IServer.h"
#include "Job.h"
#include "Session.h"
#include "GroupBase.h"
#include "GroupJob.h"

#define SESSION_IDX(id) ((id) & SESSION_INDEX_MASK)
#define GENERATE_SESION_ID(uniqueKey, sessionIdx) ((uniqueKey) << SESSION_IDX_BIT | (sessionIdx))

using namespace netlib;

netlib::SessionManager::SessionManager(IServer* owner, int32 maxSession)
	: _sessionCount(0)
	, _uniqueKey(0)
	, _owner(owner)
{
	_sessions = new Session[maxSession];
	for (int32 idx = 0; idx < maxSession; idx++)
	{
		_sessions[idx]._owner = this;
		_sessions[idx]._usageCount = 0x80000000;
		_freeIndices.Push(idx);
	}
}

netlib::SessionManager::~SessionManager()
{
	delete _sessions;
}

Session* netlib::SessionManager::Acquire(uint64 sessionId)
{
	Session& session = _sessions[SESSION_IDX(sessionId)];

	session.IncUsage();

	if (session.IsReleased() == true)
	{
		session.DecUsage();
		return nullptr;
	}

	if (sessionId != session.id)
	{
		session.DecUsage();
		return nullptr;
	}

	// Problem 1.
	// sessionId체크를 먼저했더니, 해제된 세션을 상대로 acquire가 성공하고 그과정에서, 세션이 새로생성되면, 이상한 세션으로 패킷을 보냄

	// Problem 2.
	// 따라서 체크 순서를 반대로 바꾸어,
	// Create할때는 sessionId세팅 -> useCount에서 release Flag풀기 순으로 진행하고
	// Acquire할때는 release Flag를 먼저 체크하는 식으로하면, Problem 1의 상황을 해결할 수 있음.

	// Problem 3.
	// Create할때, useCount를 1로 만들어주는데, 이것이 Interlock이 아니므로 씹힐 수가 있음. 그래서 UseCount가 -가 될수 있다.
	// 따라서 InterlockedAnd로 release를 풀어주고 Increment로 1증가해주는 방식으로 변경함.


	return &session;
}

void netlib::SessionManager::Release(Session& session)
{
	session.DecUsage();
}

void netlib::SessionManager::Return(Session& session)
{

	uint64 sessionId = session.id;


	if (session.groupId != -2)
	{
		if (session.groupId != GROUP_NULL)
		{
			GroupBase* group = static_cast<HeteroServerBase*>(_owner)->_groups[session.groupId];
			group->EnqueueJob(Job::Alloc<GroupJob_LeaveSession>(group, sessionId, true));
		}
		session.groupId = GROUP_NULL;
	}
	else
	{
		_owner->OnDisconnect(sessionId);
	}

	_freeIndices.Push(SESSION_IDX(sessionId));
	InterlockedIncrement(reinterpret_cast<LONG*>(&_releaseTotal));
	InterlockedDecrement(reinterpret_cast<LONG*>(&_sessionCount));
}

Session& netlib::SessionManager::Create(SOCKET sock, SOCKADDR_IN addr)
{
	uint32 sessionIdx = 1;
	uint64 key = _uniqueKey++;
	_freeIndices.Pop(sessionIdx);

	const uint64 sessionId = GENERATE_SESION_ID(key, sessionIdx);

	Session& ret = _sessions[sessionIdx];

	//CRASH_ASSERT(ret._usageCount == 0x80000000);

	ret.id					= sessionId;
	ret.socket				= sock;
	ret.netInfo				= NetworkAddress(addr);
	ret.IncUsage();
	InterlockedAnd((LONG*)&ret._usageCount, 0x7FFFFFFF);  /// And를 먼저하면, IncUsage사이에 Acquire가 들어와서 해제가 될 수 있음.
	ret._isRecvRestricted	= false;
	ret._isSendRestricted	= false;
	InterlockedIncrement(reinterpret_cast<LONG*>(&_sessionCount));

	return ret;
}