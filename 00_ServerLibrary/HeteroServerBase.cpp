#include "stdafx.h"

#include "GroupBase.h"
#include "Job.h"
#include "RawPacket.h"
#include "Socket.h"
#include "NetworkAddress.h"
#include "SendBuffer.h"
#include "Session.h"
#include "Thread.h"
#include "GroupJob.h"
#include "HeteroServerBase.h"


#define SESSION_IDX(id) ((id) & SESSION_INDEX_MASK)
#define GENERATE_SESION_ID(uniqueKey, sessionIdx) ((uniqueKey) << SESSION_IDX_BIT | (sessionIdx))



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief Constructor
///
/// @param addr					���ε��� ��Ʈ��ũ �ּҿ� ��Ʈ 
/// @param numOfWorkerThreads	��Ŀ������ ����
/// @param concurrentThreads	IOCP concurrent thread ����
/// @param maximumSession		�ִ� ���� �� �ִ� ���� ��
///
//-----------------------------------------------------------------------------
netlib::HeteroServerBase::HeteroServerBase(NetworkAddress addr, int32 numOfWorkerThreads, int32 concurrentThreads, uint32 maximumSession)
	: _iocp(concurrentThreads)
	, _enableNagleAlgorithm(true)
	, _numOfWorkerThreads(numOfWorkerThreads)
	, _threadHandles(new HANDLE[_numOfWorkerThreads + 1])
	, _evThreadsRun(CreateEvent(nullptr, true, false, nullptr))
	, _maxSession(maximumSession)
	, _port(addr.GetPort())
	, _sessionManager(this, _maxSession)
{
	/// WinSocket �ʱ�ȭ
	Socket::Init();

	/// Listen Socket ����
	_listenSocket = Socket::Create();
	if (Socket::Bind(_listenSocket, addr) == false)
	{
		int32 err = WSAGetLastError();
		CRASH_LOG(L"Server", L"Bind Fail port : %d, error code : %d", _port, err);
	}

	/// Thread ����
	for (int32 threadIdx = 0; threadIdx < _numOfWorkerThreads; threadIdx++)
	{
		_threadHandles[threadIdx] = RunThread(&HeteroServerBase::func_worker_thread, this);
	}
	_threadHandles[_numOfWorkerThreads] = RunThread(&HeteroServerBase::func_accept_thread, this);
}

//=============================================================================
//-----------------------------------------------------------------------------
/// @brief Destructor
///
//-----------------------------------------------------------------------------
netlib::HeteroServerBase::~HeteroServerBase()
{
	/// Thread Join
	WaitForMultipleObjects(_numOfWorkerThreads + 2, _threadHandles, true, INFINITE);

	delete[] _threadHandles;
	Socket::CleanUp();
	CloseHandle(_evThreadsRun);
}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief  ������ ������ ������ �� �ִ� ���·� �����.\n
///			������ ���� �� Listen���·� ����
///
//-----------------------------------------------------------------------------
bool netlib::HeteroServerBase::Start()
{
	/// ���� Listen ���·� ��ȯ
	if (Socket::Listen(_listenSocket) == false)
	{
		int32 err = WSAGetLastError();
		CRASH_LOG(L"Server", L"Listen Fail, error code : %d", err);
	}

	/// thread ����
	SetEvent(_evThreadsRun);

	OnStart();

	return true;
}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief ������ �����Ѵ�.
///
//-----------------------------------------------------------------------------
void netlib::HeteroServerBase::Stop()
{

	OnStop();
	/// ������ ���� �޽��� ����
	_iocp.Post(NULL, NULL, nullptr);

	_isTimerThreadExit = true; // Ÿ�̸� ������ ����
	Socket::Close(_listenSocket);

}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief	���� ������ ���´�.
///
/// @param	sessionId �����ų ���� ID
/// @return ���� ���� ���� ����
///
//-----------------------------------------------------------------------------
bool netlib::HeteroServerBase::Disconnect(uint64 sessionId)
{
	Session* session = _sessionManager.Acquire(sessionId);

	if (session == nullptr)
	{
		return false;
	}

	session->TryLockRecv();
	session->TryLockSend();
	CancelIoEx(reinterpret_cast<HANDLE>(session->socket), &session->recvOverlap);
	CancelIoEx(reinterpret_cast<HANDLE>(session->socket), &session->sendOverlap);

	_sessionManager.Release(*session);

	return true;
}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief		�ش� �������� ��Ŷ�� ������.
///
/// @param		sessionId ��Ŷ�� ���� ���� ID
/// @param		pkt ���� ��Ŷ
/// @return		���� ���� ����
///
//-----------------------------------------------------------------------------
bool netlib::HeteroServerBase::SendPacket(uint64 sessionId, Packet pkt)
{
	//SCOPE_PROFILE("Send Packet");


	Session* session = _sessionManager.Acquire(sessionId);
	if (session == nullptr)
	{
		return false;
	}

	pkt->IncRef();

	if (_enablePacketEncoding == true)
	{
		pkt->InsertNetHeader(_packetCode);
		pkt->Encode(_packetEncodingKey);
	}
	else
	{
		pkt->InsertLanHeader();
	}

	session->sendBuffer.InsertPacket(pkt.Get());

	_sessionManager.Release(*session);

	return true;
}

void netlib::HeteroServerBase::EnablePacketEncoding(BYTE code, BYTE packetKey)
{
	_enablePacketEncoding = true;
	_packetCode = code;
	_packetEncodingKey = packetKey;
}


//=============================================================================
//-----------------------------------------------------------------------------
/// @brief		�������� Network Send
///	@details	WSASend�� �ߺ� ȣ���� �Ұ����ϵ��� �����Ͽ���.
///
/// @param		session ���� ����
///
//-----------------------------------------------------------------------------
bool netlib::HeteroServerBase::send_post(Session& session)
{
	SendBuffer& sendBuf = session.sendBuffer;

	if (session.TryLockSend() == false)
	{
		return false;
	}

	if (sendBuf.Size() == 0)
	{
		session.UnlockSend();
		return false;
	}

	//UseCount 1����
	//CRASH_ASSERT(session.IncUsage() > 1);
	session.IncUsage();

	WSABUF wsabufs[256];

	int32 sendCnt = sendBuf.PrepareSend(wsabufs, 256);

	//CRASH_ASSERT(sendCnt != 0);

	const int32 sendResult = send(session, wsabufs, sendCnt);

	if (sendResult == SOCKET_ERROR)
	{
		int32 err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			session.DecUsage();
		}
	}


	return true;
}


void netlib::HeteroServerBase::SetGroup(uint64 sessionId, int8 dest)
{
	Session* session = _sessionManager.Acquire(sessionId);

	if (session == nullptr)
	{
		return;
	}


	GroupBase* groupDest = _groups[dest];
	InterlockedExchange8(&session->groupId, dest);
	groupDest->EnqueueJob(Job::Alloc<GroupJob_EnterSession>(groupDest, sessionId));

	_sessionManager.Release(*session);
}

int32 netlib::HeteroServerBase::recv(Session& session, LPWSABUF wsabuf, DWORD cnt)
{
	DWORD flag = 0;
	ZeroMemory(&session.recvOverlap, sizeof(OVERLAPPED));
	return ::WSARecv(session.socket, wsabuf, cnt, nullptr, &flag, &session.recvOverlap, NULL);
}

int32 netlib::HeteroServerBase::send(Session& session, LPWSABUF wsabuf, DWORD cnt)
{
	ZeroMemory(&session.sendOverlap, sizeof(OVERLAPPED));
	return ::WSASend(session.socket, wsabuf, cnt, nullptr, NULL, &session.sendOverlap, NULL);
}

//=============================================================================
//-----------------------------------------------------------------------------
/// @brief �������� Network Recv
///
/// @param session ���� ����
///
//-----------------------------------------------------------------------------
void netlib::HeteroServerBase::recv_post(Session& session)
{

	RecvBuffer& recvBuf = session.recvBuffer;

	WSABUF wsabuf[2];
	int32 bufCnt = recvBuf.PrepareRecv(wsabuf);

	const int32 recvResult = recv(session, wsabuf, bufCnt);

	if (recvResult == SOCKET_ERROR)
	{
		int32 err = WSAGetLastError();

		if (err != WSA_IO_PENDING)
		{
			session.DecUsage();
			return;
		}
	}

	if (session.IsRecvRestricted() == true)
	{
		CancelIoEx(reinterpret_cast<HANDLE>(session.socket), &session.recvOverlap);
	}
}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief Send �Ϸ����� ���� �Լ�
///
/// @param session �ش� Send �۾��� ����� ����
/// @param transferred Send�� ������ bytes ��
///
//-----------------------------------------------------------------------------
void netlib::HeteroServerBase::handle_send_completion(Session& session, uint32 transferred)
{
	SendBuffer& sendBuf = session.sendBuffer;

	int32 sentSize = sendBuf.FreeSentPackets();

	InterlockedAdd(reinterpret_cast<LONG*>(&_sendMessageCnt), sentSize);

	session.DecUsage();
	//CRASH_ASSERT(session.UnlockSend() == true);
	session.UnlockSend();
	// sendBuf�� ���� ���� ���� �����մٸ�, Send ���û
	if (sendBuf.IsEmpty() == false)
	{
		send_post(session);
	}
}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief Recv �Ϸ����� ���� �Լ�
///
/// @param session �ش� Recv �۾��� ����� ����
/// @param transferred Recv�� ������ bytes ��
///
//-----------------------------------------------------------------------------
void netlib::HeteroServerBase::handle_recv_completion(Session& session, uint32 transferred)
{
	RecvBuffer& recvBuf = session.recvBuffer;

	recvBuf.MoveRear(transferred);

	int32 recvSize = 0;

	if (_enablePacketEncoding == true)
	{
		while (recvBuf.CanExtractNetPacket())
		{
			RawPacket& pkt = RawPacket::Alloc();

			int32 result = recvBuf.ExtractNetPacket(pkt, _packetCode, _packetEncodingKey);

			if (result != SUCCESS_EXTRACT)
			{
				//CRASH_LOG(L"Error", L"RecvBuf Overflow");
				LOG_DBG(L"Error", L"Recv Error code : %d", result);
				Disconnect(session.id);
				return;
			}
			pkt.IncRef();

			session.messegeQueue.Push(&pkt);
			recvSize++;
		}
	}
	else
	{
		while (recvBuf.CanExtractLanPacket())
		{
			RawPacket& pkt = RawPacket::Alloc();
			int32 result = recvBuf.ExtractLanPacket(pkt);

			if (result != SUCCESS_EXTRACT)
			{
				//CRASH_LOG(L"Error", L"RecvBuf Overflow");
				LOG_DBG(L"Error", L"Recv Error code : %d", result);
				Disconnect(session.id);
				return;
			}

			pkt.IncRef();
			session.messegeQueue.Push(&pkt);
			recvSize++;
		}
	}

	InterlockedAdd64(reinterpret_cast<LONG64*>(&_recvMessageCnt), recvSize);

	// ������ ������ �ѹ��� ���ú�ó���� ������� ����
	recv_post(session);
}


//=============================================================================
//-----------------------------------------------------------------------------
/// @brief ��Ŀ������ ���� �Լ�
///
/// @param param this
///
//-----------------------------------------------------------------------------
void netlib::HeteroServerBase::func_worker_thread()
{
	WaitForSingleObject(_evThreadsRun, INFINITE);
	while (true)
	{
		DWORD transferred = 0;
		ULONG_PTR sessionId = 0;
		OVERLAPPED* overlapped = nullptr;

		const bool iocpResult = _iocp.Dispatch(&transferred, &sessionId, &overlapped);

		// ���� ����
		if (transferred == 0 && overlapped == nullptr && sessionId == 0)
		{
			_iocp.Post(0, 0, nullptr);
			break;
		}

		Session* session = _sessionManager.Acquire(sessionId);

		if (session == nullptr)
		{
			continue;
		}

		if (iocpResult == false)
		{
			// UseCount 1����
			session->DecUsage();
		}
		else if (transferred == 0)
		{
			// UseCount 1����
			session->DecUsage();
		}
		else if (transferred == 0xFFFFFFFF)
		{
			send_post(*session);
		}
		else
		{
			/// Recv �Ϸ�����
			if (overlapped == &session->recvOverlap)
			{
				handle_recv_completion(*session, transferred);
			}
			/// Send �Ϸ�����
			else
			{
				handle_send_completion(*session, transferred);
			}
		}

		_sessionManager.Release(*session);

	}
}



//=============================================================================
//-----------------------------------------------------------------------------
/// @brief Accept ������ ���� �Լ�
///
/// @param param this
///
//-----------------------------------------------------------------------------
void netlib::HeteroServerBase::func_accept_thread()
{
	WaitForSingleObject(_evThreadsRun, INFINITE);
	while (true)
	{
		SOCKADDR_IN clientAddr;
		int32 addrLen = sizeof(clientAddr);

		const SOCKET clientSocket = ::accept(_listenSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);

		if (clientSocket == INVALID_SOCKET)
		{
			break;
		}

		//Socket::SetSendBufferSize(clientSocket, 0); /// ���Ź��� ������ 0���� ����
		Socket::SetLinger(clientSocket, 1, 0); /// Linger 0 ����
		if (_enableNagleAlgorithm == false)
		{
			Socket::SetNagle(clientSocket, false);
		}


		// �ִ� ���� ���� ����� �ٷ� �������
		if (_sessionManager.GetSessionCount() >= _maxSession)
		{
			Socket::Close(clientSocket);
			continue;
		}

		const NetworkAddress netInfo(clientAddr);

		if (OnConnectionRequest(netInfo) == false)
		{
			Socket::Close(clientSocket);
			continue;
		}

		Session& newSession = _sessionManager.Create(clientSocket, clientAddr);
		/// IOCP ���
		_iocp.Register(newSession);

		OnAccept(netInfo, newSession.id);

		recv_post(newSession);

		InterlockedIncrement64(reinterpret_cast<LONG64*>(&_acceptCnt));

	}
}


#undef SESSION_ID_TOOL
