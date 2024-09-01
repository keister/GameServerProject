#include "stdafx.h"
#include "SendBuffer.h"

#include "RawPacket.h"

netlib::SendBuffer::~SendBuffer()
{
	while (IsEmpty() == false)
	{
		RawPacket* pkt;
		if (_packetQueue.Pop(pkt) == false)
		{
			break;
		}

		pkt->DecRef();
	}

	while (!_sentPackets.empty())
	{
		RawPacket& pkt = *_sentPackets.front();
		_sentPackets.pop();
		pkt.DecRef();
	}
}


int32 netlib::SendBuffer::PrepareSend(WSABUF* wsabufs, int32 len)
{
	int32 cnt = 0;
	//CRASH_ASSERT(cnt == _sentPackets.size());
	int64 queueSize = _packetQueue.Size();
	for (int32 idx = 0; idx < len; idx++)
	{
		if (queueSize <= 0)
			break;

		RawPacket* pkt;
		if (_packetQueue.Pop(pkt) == false)
		{
			break;
		}

		wsabufs[idx].buf = (char*)pkt->GetBufferPtr();
		wsabufs[idx].len = pkt->GetDataSize();
		_sentPackets.push(pkt);
		cnt++;
		queueSize--;
	}

	//CRASH_ASSERT(cnt != 0);

	return cnt;
}

int32 netlib::SendBuffer::FreeSentPackets()
{
	int32 size = 0;
	while (!_sentPackets.empty())
	{
		RawPacket& pkt = *_sentPackets.front();
		_sentPackets.pop();
		pkt.DecRef();
		size++;
	}
	return size;
}

void netlib::SendBuffer::Clear()
{
	while (IsEmpty() == false)
	{
		RawPacket* pkt;
		if (_packetQueue.Pop(pkt) == false)
		{
			break;
		}

		pkt->DecRef();
	}

	while (!_sentPackets.empty())
	{
		RawPacket& pkt = *_sentPackets.front();
		_sentPackets.pop();
		pkt.DecRef();
	}
}
