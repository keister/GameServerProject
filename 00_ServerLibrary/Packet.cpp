#include "stdafx.h"
#include "Packet.h"
#include "RawPacket.h"

netlib::Packet netlib::Packet::Alloc()
{
	return {&RawPacket::Alloc()};
}

netlib::Packet::Packet(RawPacket* ptr)
	: _ptr(ptr)
{
	_ptr->IncRef();
}

netlib::Packet::Packet(const Packet& rhs)
	: _ptr(rhs._ptr)
{
	_ptr->IncRef();
}

netlib::Packet::~Packet()
{
	_ptr->DecRef();
}

netlib::Packet& netlib::Packet::operator=(const Packet& rhs)
{
	_ptr = rhs._ptr;
	_ptr->IncRef();

	return *this;
}
