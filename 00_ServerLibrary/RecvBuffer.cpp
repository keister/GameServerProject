#include "stdafx.h"
#include "RecvBuffer.h"
#include "RawPacket.h"
#include "Profiler.h"

int32 netlib::RecvBuffer::PrepareRecv(WSABUF* wsabufs)
{
	int32 bufCnt = 1;
	int32 directSize = _buffer.DirectEnqueueSize();

	wsabufs[0] = { static_cast<ULONG>(directSize), reinterpret_cast<CHAR*>(_buffer.GetRearBufferPtr()) };

	int32 remainSize = _buffer.GetFreeSize() - directSize;
	if (remainSize != 0)
	{
		bufCnt = 2;
		wsabufs[1].len = remainSize;
		wsabufs[1].buf = reinterpret_cast<CHAR*>(_buffer.GetBeginBufferPtr());
	}

	return bufCnt;
}

bool netlib::RecvBuffer::CanExtractLanPacket()
{
	if (_buffer.GetUseSize() < sizeof(LanPacketHeader))
	{
		return false;
	}

	LanPacketHeader header;
	_buffer.Peek(reinterpret_cast<BYTE*>(&header), sizeof(LanPacketHeader));
	if (_buffer.GetUseSize() < header.size + sizeof(LanPacketHeader))
	{
		return false;
	}


	return true;
}

bool netlib::RecvBuffer::CanExtractNetPacket()
{
	int32 size = _buffer.GetUseSize();

	if (size < sizeof(NetPacketHeader))
	{
		return false;
	}

	NetPacketHeader header;
	_buffer.Peek(reinterpret_cast<BYTE*>(&header), sizeof(NetPacketHeader));
	if (size < header.size + sizeof(NetPacketHeader))
	{
		return false;
	}


	return true;
}

int32 netlib::RecvBuffer::ExtractLanPacket(RawPacket& pkt)
{
	int32 size = _buffer.GetUseSize();


	LanPacketHeader header;

	_buffer.Peek(reinterpret_cast<BYTE*>(&header), sizeof(LanPacketHeader));
	pkt.MoveReadPos(-(int32)sizeof(LanPacketHeader));

	int32 dequeueResult = _buffer.Dequeue(pkt.GetBufferPtr(), header.size + sizeof(LanPacketHeader));
	pkt.MoveWritePos(header.size);

	if (dequeueResult == -1)
	{
		return BUFFER_OVERFLOW;
	}

	pkt.MoveReadPos(sizeof(LanPacketHeader));
	return SUCCESS_EXTRACT;
}

int32 netlib::RecvBuffer::ExtractNetPacket(RawPacket& pkt, BYTE code, BYTE key)
{
	int32 size = _buffer.GetUseSize();
	NetPacketHeader header;


	_buffer.Peek(reinterpret_cast<BYTE*>(&header), sizeof(NetPacketHeader));

	if (code != header.code)
	{
		return PACKET_CODE_NOT_MATCH;
	}

	pkt.MoveReadPos(-(int32)sizeof(NetPacketHeader));

	int32 dequeueResult = _buffer.Dequeue(pkt.GetBufferPtr(), header.size + sizeof(NetPacketHeader));

	pkt.MoveWritePos(header.size);

	if (dequeueResult == -1)
	{
		return BUFFER_OVERFLOW;
	}

	if (pkt.Decode(key) == false)
	{
		return FAIL_PACKET_DECODE;
	}

	pkt.MoveReadPos(sizeof(NetPacketHeader));
	return SUCCESS_EXTRACT;
}
