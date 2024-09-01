#pragma once
#include "LockFreeStack.h"
#include "Session.h"



namespace netlib
{
	class IServer;
	class ServerBase;
	class SessionManager
	{
		enum : uint64
		{
			RECV_BUFFER_SIZE = 5000,
			SESSION_IDX_BIT = 16,
			SESSION_ID_BIT = 48,
			SESSION_INDEX_MASK = UINT64_MAX >> SESSION_ID_BIT,
			SESSION_ID_MASK = UINT64_MAX << SESSION_IDX_BIT,

			USAGE_COUNT_BIT = 31,
		};

	public:
		SessionManager() = delete;
		SessionManager(IServer* _owner, int32 maxSession);
		~SessionManager();

		Session& Create(SOCKET sock, SOCKADDR_IN addr);
		Session* Acquire(uint64 sessionId);
		void		Release(Session& session);
		void		Return(Session& session);

		uint64		GetSessionCount() { return _sessionCount; }
		uint64		GetReleaseTotal() { return _releaseTotal; }


	private:
		uint64					_sessionCount;
		uint64					_releaseTotal = 0;
		uint64					_uniqueKey;
		Session* _sessions;
		LockFreeStack<uint32>	_freeIndices;
		IServer* _owner;
	};
}




