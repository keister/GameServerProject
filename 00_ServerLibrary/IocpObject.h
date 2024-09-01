#pragma once
#include "Session.h"


namespace netlib
{
	class IocpObject
	{
	public:
		IocpObject(int32 concurrentThreads)
			: _hCompletionPort(::CreateIoCompletionPort(INVALID_HANDLE_VALUE, nullptr, NULL, concurrentThreads))
		{}

		~IocpObject()
		{
			CloseHandle(_hCompletionPort);
		}

		void Register(HANDLE handle, uint64 key)
		{
			::CreateIoCompletionPort(handle, _hCompletionPort, key, NULL);
		}

		void Register(Session& session)
		{
			::CreateIoCompletionPort((HANDLE)session.socket, _hCompletionPort, session.id, NULL);
		}

		bool Dispatch(DWORD* transferred, ULONG_PTR* completionKey, OVERLAPPED** overlapped, uint32 timeoutMs = INFINITE)
		{
			return ::GetQueuedCompletionStatus(
				IN	_hCompletionPort,
				OUT transferred,
				OUT completionKey,
				OUT overlapped,
				IN	timeoutMs
			);
		}

		bool Post(uint32 transferred, uint64 key, OVERLAPPED* ovelapped)
		{
			return ::PostQueuedCompletionStatus(_hCompletionPort, transferred, key, ovelapped);
		}
	private:
		HANDLE _hCompletionPort;

	};
}


