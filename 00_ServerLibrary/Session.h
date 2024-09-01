#pragma once
#include "NetworkAddress.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"




enum class SessionState
{
	CONNECT,
	MOVE
};

namespace netlib
{
	class SessionManager;
	class GroupBase;

	class Session
	{
		friend SessionManager;

		enum : ULONG
		{
			CAN_RELEASE = 0x0000'0000,
			LOCK_RELEASE = 0x8000'0000,
			USAGE_COUNT_MASK = 0x7FFF'FFFF,
			RELEASE_FLAG_MASK = 0x8000'0000,
		};

	public:
		int32 IncUsage()
		{
			int32 ret = InterlockedIncrement((LONG*)&_usageCount) & USAGE_COUNT_MASK;

			//CRASH_ASSERT(ret >= 0);

			return ret;
		}

		int32 DecUsage()
		{
			int32 ret = InterlockedDecrement(reinterpret_cast<LONG*>(&_usageCount)) & USAGE_COUNT_MASK;

			//CRASH_ASSERT(ret >= 0);
			if (ret == 0)
			{
				release();
			}

			return ret;
		}


		bool TryLockSend()
		{
			return InterlockedExchange8(reinterpret_cast<CHAR*>(&_isSendRestricted), true) == false;
		}

		bool UnlockSend()
		{
			return InterlockedExchange8(reinterpret_cast<CHAR*>(&_isSendRestricted), false);
		}

		bool TryLockRecv()
		{
			return InterlockedExchange8(reinterpret_cast<CHAR*>(&_isRecvRestricted), true) == false;
		}

		bool UnlockRecv()
		{
			return InterlockedExchange8(reinterpret_cast<CHAR*>(&_isRecvRestricted), false);
		}

		bool IsRecvRestricted() { return _isRecvRestricted; }
		bool IsSendRestricted() { return _isSendRestricted; }
		bool IsReleased() { return (_usageCount & RELEASE_FLAG_MASK) != 0; }

		const NetworkAddress& GetNetInfo() { return netInfo; }

	private:
		Session();

		void release();

	public:
		uint64			id;
		SOCKET			socket;
		NetworkAddress	netInfo;
		int8 			groupId = -2;
		RecvBuffer		recvBuffer;
		SendBuffer		sendBuffer;
		OVERLAPPED		recvOverlap;
		OVERLAPPED		sendOverlap;
		LockFreeQueue<RawPacket*> messegeQueue;

	private:
		uint32			_usageCount;
		bool			_isSendRestricted;
		bool			_isRecvRestricted;
		SessionManager* _owner;
	};

}
