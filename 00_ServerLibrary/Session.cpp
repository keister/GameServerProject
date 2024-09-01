#include "stdafx.h"
#include "Session.h"
#include "SessionManager.h"

netlib::Session::Session()
	: id(0)
	, socket(INVALID_SOCKET)
	, netInfo()
	, recvOverlap()
	, sendOverlap()
	, _usageCount(0)
	, _isSendRestricted(false)
	, _isRecvRestricted(false)
	, _owner(nullptr)
{
}

void netlib::Session::release()
{
	if (InterlockedCompareExchange((LONG*)&_usageCount, LOCK_RELEASE, CAN_RELEASE) != CAN_RELEASE)
	{
		return;
	}
	recvBuffer.Clear();
	sendBuffer.Clear();
	closesocket(socket);
	_owner->Return(*this);
}
