#pragma once

#include <functional>

#include "HeteroServerBase.h"


namespace netlib
{
	class Job;
	class RawPacket;
	class JobQueue;

	class GroupBase
	{
		friend class GroupJob_EnterSession;
		friend class GroupJob_LeaveSession;
		friend HeteroServerBase;
	public:
		GroupBase();
		virtual ~GroupBase();

		bool Disconnect(uint64 sessionId)
		{
			return _server->Disconnect(sessionId);
		}
		bool SendPacket(uint64 sessionId, Packet pkt)
		{
			return _server->SendPacket(sessionId, pkt);
		}

		void Start() { SetEvent(_evThreadStart); }

		void EnqueueJob(Job* job);

		float32 DeltaTime()
		{
			return (float32)_deltaTick / 1000;
		}

		DWORD CurrentTick()
		{
			return _currentTick;
		}

		DWORD DeltaTick()
		{
			return _deltaTick;
		}

		uint64 GetSessionCount() { return _sessionList.size(); }
		uint64 GetFrameCount() { return _frameCount; }
		uint8 GetGroupIndex() { return _groupIndex; }

	protected:
		virtual void Init() {}
		virtual void OnEnter(uint64 sessionId) = 0;
		virtual void OnLeave(uint64 sessionId) = 0;
		virtual void OnRecv(uint64 sessionId, Packet packet) = 0;
		virtual void UpdateFrame() = 0;

		void MoveGroup(uint64 sessionId, int8 dest);

	private:
		void thread_func();

		void process_job();
		void process_packet();

		void set_server(HeteroServerBase* server) { _server = server; }
		void set_fps(uint32 fps) { _fps = fps; }
		void set_idx(int8 idx) { _groupIndex = idx; }
		void on_disconnect(uint64 sessionId) { _server->OnDisconnect(sessionId); }

		bool reserve_erase(uint64 sessionId);
	private:
		uint32		_fps;
		JobQueue* _jobQueue;
		HeteroServerBase* _server;
		HANDLE		_evThreadStart;
		HANDLE		_threadHandle;
		int8		_groupIndex;
		uint64		_frameCount;

		unordered_set<uint64> _sessionList;
		list<uint64> _eraseList;

		DWORD _deltaTick;
		DWORD _currentTick;


	};

	inline bool GroupBase::reserve_erase(uint64 sessionId)
	{
		auto it = _sessionList.find(sessionId);

		if (it == _sessionList.end())
		{
			return false;
		}

		_eraseList.push_back(sessionId);
		return true;
	}



}
