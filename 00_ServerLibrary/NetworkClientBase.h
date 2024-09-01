#pragma once
#include "IOCPObject.h"
#include "NetworkAddress.h"
#include "SessionManager.h"
#include "RawPacket.h"


namespace netlib
{
	class NetworkClientBase
	{
	public:
		NetworkClientBase(NetworkAddress address, int32 numWorkerThreads, int32 concurrentThreads, bool enableNagleAlgorithm);
		NetworkClientBase(NetworkAddress address, int32 numWorkerThreads, int32 concurrentThreads, bool enableNagleAlgorithm, BYTE code, BYTE key);
		virtual ~NetworkClientBase();


		bool Connect();
		bool Disconnect();
		bool SendPacket(RawPacket& pkt);



	protected:
		virtual void OnConnect() = 0;
		virtual void OnDisconnect() = 0;
		virtual void OnRecv(RawPacket& pkt) = 0;

	private:
		bool		recv_post();
		bool		send_post();

		void		handle_recv_completion(uint32 transferred);
		void		handle_send_completion();

		void		func_worker_thread();

	private:
		IocpObject		_iocp;
		bool			_enableNagleAlgorithm;
		int32			_numOfWorkerThreads;	///< 워커 스레드 수
		HANDLE*			_threadHandles;			///< 스레드 핸들 관리하는 배열
		bool			_enablePacketEncoding;	///< 패킷 암호화 여부
		BYTE			_packetCode = 0;
		BYTE			_packetEncodingKey = 0;
		HANDLE			_evThreadsRun;

		NetworkAddress	_serverAddress;
		SOCKET			_socket;
		RecvBuffer		_recvBuffer;
		SendBuffer		_sendBuffer;
		OVERLAPPED		_recvOverlap;
		OVERLAPPED		_sendOverlap;
		bool			_isSendRestricted = false;
	};

	
}
