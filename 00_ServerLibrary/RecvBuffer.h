#pragma once
#include "RingBuffer.h"
#define SUCCESS_EXTRACT			0x00
#define FAIL_EXTRACT			0x01
#define PACKET_CODE_NOT_MATCH	0x02
#define FAIL_PACKET_DECODE		0x03
#define BUFFER_OVERFLOW			0x04


namespace netlib
{
	class ServerBase;
	class RawPacket;
	class RecvBuffer
	{
	public:
		int32	PrepareRecv(WSABUF* wsabufs);
		void	MoveRear(uint32 size) { _buffer.MoveRear(size); }

		void	Clear() { _buffer.Clear(); }
		bool	CanExtractLanPacket();
		bool	CanExtractNetPacket();

		int32	ExtractLanPacket(RawPacket& pkt);
		int32   ExtractNetPacket(RawPacket& pkt, BYTE code, BYTE key);

		int32	Size() { return _buffer.GetUseSize(); }

	private:
		RingBuffer	_buffer;
	};

	class NetRecvBuffer
	{

	};




}





