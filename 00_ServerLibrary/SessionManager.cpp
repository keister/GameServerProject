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
	// sessionIdüũ�� �����ߴ���, ������ ������ ���� acquire�� �����ϰ� �װ�������, ������ ���λ����Ǹ�, �̻��� �������� ��Ŷ�� ����

	// Problem 2.
	// ���� üũ ������ �ݴ�� �ٲپ�,
	// Create�Ҷ��� sessionId���� -> useCount���� release FlagǮ�� ������ �����ϰ�
	// Acquire�Ҷ��� release Flag�� ���� üũ�ϴ� �������ϸ�, Problem 1�� ��Ȳ�� �ذ��� �� ����.

	// Problem 3.
	// Create�Ҷ�, useCount�� 1�� ������ִµ�, �̰��� Interlock�� �ƴϹǷ� ���� ���� ����. �׷��� UseCount�� -�� �ɼ� �ִ�.
	// ���� InterlockedAnd�� release�� Ǯ���ְ� Increment�� 1�������ִ� ������� ������.


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
	InterlockedAnd((LONG*)&ret._usageCount, 0x7FFFFFFF);  /// And�� �����ϸ�, IncUsage���̿� Acquire�� ���ͼ� ������ �� �� ����.
	ret._isRecvRestricted	= false;
	ret._isSendRestricted	= false;
	InterlockedIncrement(reinterpret_cast<LONG*>(&_sessionCount));

	return ret;
}