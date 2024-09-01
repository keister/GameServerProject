#pragma once
#include "LockFreeQueue.h"

namespace netlib
{
	class ServerBase;

	class RawPacket;

	class SendBuffer
	{
	public:
		~SendBuffer();


		void InsertPacket(RawPacket& pkt) { _packetQueue.Push(&pkt); };
		int32 PrepareSend(WSABUF* wsabufs, int32 len);
		int32 FreeSentPackets();
		void Clear();
		bool IsEmpty() { return _packetQueue.Size() <= 0; }
		int32	Size() { return _packetQueue.Size(); }

	private:
		LockFreeQueue<RawPacket*>	_packetQueue;
		queue<RawPacket*>			_sentPackets;
	};
}



