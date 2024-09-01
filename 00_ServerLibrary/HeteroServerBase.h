#pragma once


#include "NetworkAddress.h"
#include "IOCPObject.h"
#include "IServer.h"
#include "Packet.h"
#include "SessionManager.h"

#define GROUP_NULL -1


namespace netlib
{
	class RingBuffer;
	class RawPacket;
	class HeteroServerBase : public IServer
	{
		friend class GroupJob_EnterSession;
		friend class GroupJob_LeaveSession;
		friend SessionManager;
		friend class GroupBase;

	public:
		//------------------------------------------------------------
		// Constructor And Destructor --------------------------------
		HeteroServerBase(
			NetworkAddress	address,
			int32			numOfWorkerThreads,
			int32			concurrentThreads,
			uint32			maximumSession
		);

		virtual ~HeteroServerBase();

		bool	Start();
		void	Stop();

		bool	Disconnect(uint64 sessionId);
		bool	SendPacket(uint64 sessionId, Packet pkt);

		void	EnablePacketEncoding(BYTE code, BYTE packetKey);
		void	DisableNagleAlgorithm() { _enableNagleAlgorithm = false; }
		//------------------------------------------------------------
		// 모니터링 변수 Getter --------------------------------------
		uint64	GetAcceptCount() { return _acceptCnt; }
		uint64	GetRecvMessageCount() { return _recvMessageCnt; }
		uint64	GetSendMessageCount() { return _sendMessageCnt; }
		uint64	GetSessionCount() { return _sessionManager.GetSessionCount(); }
		uint64	GetReleaseTotal() { return _sessionManager.GetReleaseTotal(); }
		uint16	GetServerPort() { return _port; }

	protected:
		void SetGroup(uint64 sessionId, int8 dest);
		template <typename T> requires is_base_of_v<GroupBase, T>
		T* CreateGroup(int8 groupIndex, int32 fps = 0);

	private:
		int32	   recv(Session& session, LPWSABUF wsabuf, DWORD cnt);
		int32	   send(Session& session, LPWSABUF wsabuf, DWORD cnt);


		//------------------------------------------------------------
		// Network 송수신 함수 ---------------------------------------
		void		recv_post(Session& session);
		bool		send_post(Session& session);

		//------------------------------------------------------------
		// 완료 통지 처리 함수 ---------------------------------------
		void		handle_recv_completion(Session& session, uint32 transferred);
		void		handle_send_completion(Session& session, uint32 transferred);

		//------------------------------------------------------------
		// Thread 관련 함수 ------------------------------------------
		void			func_worker_thread();
		void			func_accept_thread();

	private:
		// Read Only Members
		IocpObject				_iocp;					///< IOCP 객체
		SOCKET					_listenSocket;			///< 리슨 소켓
		bool					_enableNagleAlgorithm;	///< 네이글 알고리즘 여부
		int32					_numOfWorkerThreads;	///< 워커 스레드 수
		HANDLE*					_threadHandles;			///< 스레드 핸들 관리하는 배열
		HANDLE					_evThreadsRun;			///< 실제로 스레드를 동작하게 하는 이벤트
		int32					_maxSession;			///< 최대 수용가능 세션
		uint16					_port;

		bool					_enablePacketEncoding;	///< 패킷 암호화 여부
		BYTE					_packetCode = 0;
		BYTE					_packetEncodingKey = 0;

		bool					_isTimerThreadExit = false;

		alignas(64)
			uint64					_acceptCnt = 0;	///< 총 accept 처리 횟수
		uint64					_recvMessageCnt = 0;	///< 총 recv message 처리 갯수
		uint64					_sendMessageCnt = 0;	///< 총 send message 처리 갯수

		int32					_wsaSendBufMax = 0;

		SessionManager			_sessionManager;
		GroupBase* _groups[128];
	};

	template <typename T> requires is_base_of_v<GroupBase, T>
	T* HeteroServerBase::CreateGroup(int8 groupIndex, int32 fps)
	{
		if (_groups[groupIndex] != nullptr)
		{
			CRASH_LOG(L"Error", L"double index %d", groupIndex);
		}
		T* group = new T();
		group->set_server(this);
		group->set_fps(fps);
		group->set_idx(groupIndex);
		_groups[groupIndex] = group;
		group->Init();

		group->Start();

		return group;
	}


}



