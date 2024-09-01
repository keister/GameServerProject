#pragma once
#include "RawPacket.h"


namespace netlib
{

	/// @brief 직렬화버퍼의 자동 참조카운트 계산 버전
	class Packet
	{
	public:
		static Packet Alloc();
		static int32	GetUseCount() { return RawPacket::GetUseCount(); }
		static int32    GetCapacity() { return  RawPacket::GetCapacity(); }

		Packet(RawPacket* ptr);
		Packet(const Packet& rhs);
		~Packet();

		RawPacket& Get() { return *_ptr; }
		RawPacket* Raw() { return _ptr; }

		RawPacket* operator->() { return _ptr; }
		RawPacket& operator*() { return *_ptr; }
		Packet& operator=(const Packet& rhs);


	private:
		RawPacket* _ptr;
	};
}
