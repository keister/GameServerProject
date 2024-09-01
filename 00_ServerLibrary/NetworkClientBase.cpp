#include <stdafx.h>
#include "NetworkClientBase.h"

#include "Socket.h"
#include "Thread.h"

netlib::NetworkClientBase::NetworkClientBase(NetworkAddress address, int32 numWorkerThreads, int32 concurrentThreads, bool enableNagleAlgorithm)
	: _iocp(concurrentThreads)
	, _numOfWorkerThreads(numWorkerThreads)
	, _threadHandles(new HANDLE[numWorkerThreads])
	, _enablePacketEncoding(false)
	, _evThreadsRun(CreateEvent(nullptr, true, false, nullptr))
	, _serverAddress(address)
	, _enableNagleAlgorithm(enableNagleAlgorithm)
{
	/// WinSocket 초기화
	Socket::Init();

	for (int32 threadIdx = 0; threadIdx < _numOfWorkerThreads; threadIdx++)
	{
		_threadHandles[threadIdx] = RunThread(&NetworkClientBase::func_worker_thread, this);
	}
}

netlib::NetworkClientBase::NetworkClientBase(NetworkAddress address, int32 numWorkerThreads, int32 concurrentThreads, bool enableNagleAlgorithm,
                                             BYTE code, BYTE key)
	: _iocp(concurrentThreads)
	, _numOfWorkerThreads(numWorkerThreads)
	, _threadHandles(new HANDLE[numWorkerThreads])
	, _enablePacketEncoding(true)
	, _evThreadsRun(CreateEvent(nullptr, true, false, nullptr))
	, _serverAddress(address)
	, _enableNagleAlgorithm(enableNagleAlgorithm)
	, _packetCode(code)
	, _packetEncodingKey(key)
{
	Socket::Init();

	for (int32 threadIdx = 0; threadIdx < _numOfWorkerThreads; threadIdx++)
	{
		_threadHandles[threadIdx] = RunThread(&NetworkClientBase::func_worker_thread, this);
	}
}

netlib::NetworkClientBase::~NetworkClientBase()
{
}


bool netlib::NetworkClientBase::Connect()
{
	_socket = Socket::Create();
	_recvBuffer.Clear();
	_sendBuffer.Clear();

	SetEvent(_evThreadsRun);

	if (Socket::Connect(_socket, _serverAddress) == false)
	{
		int32 err = WSAGetLastError();
		return false;
	}

	_iocp.Register((HANDLE)_socket, NULL);

	OnConnect();

	recv_post();

	return true;
}

bool netlib::NetworkClientBase::Disconnect()
{
	closesocket(_socket);
	OnDisconnect();
	return false;
}

bool netlib::NetworkClientBase::SendPacket(RawPacket& pkt)
{
	pkt.IncRef();

	if (_enablePacketEncoding == true)
	{
		pkt.InsertNetHeader(_packetCode);
		pkt.Encode(_packetEncodingKey);
	}
	else
	{
		pkt.InsertLanHeader();
	}

	_sendBuffer.InsertPacket(pkt);
	send_post();

	return true;
}

bool netlib::NetworkClientBase::recv_post()
{
	WSABUF wsabuf[2];
	int32 bufCnt = _recvBuffer.PrepareRecv(wsabuf);

	DWORD flag = 0;

	ZeroMemory(&_recvOverlap, sizeof(OVERLAPPED));
	const int32 recvResult = WSARecv(_socket, wsabuf, bufCnt, nullptr, &flag, &_recvOverlap, NULL);

	if (recvResult == SOCKET_ERROR)
	{
		int32 err = WSAGetLastError();

		if (err != WSA_IO_PENDING)
		{
			Disconnect();
			return false;
		}
	}

	return true;

}

bool netlib::NetworkClientBase::send_post()
{

	if (InterlockedExchange8((char*)&_isSendRestricted, true) == true)
	{
		return false;
	}

	if (_sendBuffer.IsEmpty())
	{
		InterlockedExchange8((char*)&_isSendRestricted, false);
		return false;
	}

	WSABUF wsabufs[256];

	int32 sendCnt = _sendBuffer.PrepareSend(wsabufs, 256);

	CRASH_ASSERT(sendCnt != 0);
	ZeroMemory(&_sendOverlap, sizeof(OVERLAPPED));
	const int32 sendResult = WSASend(_socket, wsabufs, sendCnt, nullptr, NULL, &_sendOverlap, nullptr);

	if (sendResult == SOCKET_ERROR)
	{
		int32 err = WSAGetLastError();
		if (err != WSA_IO_PENDING)
		{
			Disconnect();
			return false;
		}
	}

	return true;
}

void netlib::NetworkClientBase::handle_recv_completion(uint32 transferred)
{
	_recvBuffer.MoveRear(transferred);

	if (_enablePacketEncoding == true)
	{
		while (_recvBuffer.CanExtractNetPacket())
		{
			RawPacket& pkt = RawPacket::Alloc();
			pkt.IncRef();
			if (_recvBuffer.ExtractNetPacket(pkt, _packetCode, _packetEncodingKey) != SUCCESS_EXTRACT)
			{
				CRASH("RecvBuf Overflow");
				Disconnect();
				return;
			}
			OnRecv(pkt);
			pkt.DecRef();
		}
	}
	else
	{
		while (_recvBuffer.CanExtractLanPacket())
		{
			RawPacket& pkt = RawPacket::Alloc();
			pkt.IncRef();
			if (_recvBuffer.ExtractLanPacket(pkt) != SUCCESS_EXTRACT)
			{
				CRASH("RecvBuf Overflow");
				Disconnect();
				return;
			}
			OnRecv(pkt);
			pkt.DecRef();
		}
	}

	// 위에다 놓으면 한번의 리시브처리를 보장받지 못함
	recv_post();
}

void netlib::NetworkClientBase::handle_send_completion()
{

	_sendBuffer.FreeSentPackets();

	InterlockedExchange8((char*)&_isSendRestricted, false);

	// sendBuf에 아직 보낼 것이 남아잇다면, Send 재요청
	if (_sendBuffer.IsEmpty() == false)
	{
		send_post();
	}
}

void netlib::NetworkClientBase::func_worker_thread()
{
	WaitForSingleObject(_evThreadsRun, INFINITE);
	while (true)
	{
		DWORD transferred = 0;
		ULONG_PTR none = 0;
		OVERLAPPED* overlapped = nullptr;

		const bool iocpResult = _iocp.Dispatch(&transferred, &none, &overlapped);

		// 종료 통지
		if (transferred == 0 && overlapped == nullptr && none == 0)
		{
			_iocp.Post(0, 0, nullptr);
			break;
		}

		if (iocpResult == false)
		{
			Disconnect();
		}


		if (transferred == 0)
		{
			Disconnect();
		}
		else
		{
			/// Recv 완료통지
			if (overlapped == &_recvOverlap)
			{
				handle_recv_completion(transferred);
			}
			/// Send 완료통지
			else
			{
				handle_send_completion();
			}
		}
	}
}
