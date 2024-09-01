#include "stdafx.h"
#include "RawPacket.h"


netlib::RawPacket::RawPacket()
	: _bufferSize(BUFFER_DEFAULT_SIZE)
	, _readPos(sizeof(NetPacketHeader))
	, _writePos(sizeof(NetPacketHeader))
{
	_buffer = new BYTE[_bufferSize + sizeof(NetPacketHeader)];
	
}

netlib::RawPacket::RawPacket(int32 bufferSize)
	: _bufferSize(bufferSize)
	, _readPos(sizeof(NetPacketHeader))
	, _writePos(sizeof(NetPacketHeader))
	, _refCount(0)
{
	_buffer = new BYTE[_bufferSize + sizeof(NetPacketHeader)];
}

netlib::RawPacket::~RawPacket()
{
	delete[] _buffer;
}

void netlib::RawPacket::Clear()
{
	_readPos = sizeof(NetPacketHeader);
	_writePos = sizeof(NetPacketHeader);
	_encoded = false;
	_headerHas = false;
	_refCount = 0;
}

int32 netlib::RawPacket::MoveWritePos(int32 size)
{
	_writePos += size;
	return size;
}

int32 netlib::RawPacket::MoveReadPos(int32 size)
{
	_readPos += size;
	return size;
}

int32 netlib::RawPacket::GetData(BYTE* dest, int32 size)
{
	if (_readPos + size > _writePos)
		return -1;

	::memcpy(dest, _buffer + _readPos, size);
	_readPos += size;

	return size;
}

int32 netlib::RawPacket::PutData(const BYTE* src, int32 size)
{
	if (_writePos + size > _bufferSize)
		return -1;

	::memcpy(_buffer + _writePos, src, size);
	_writePos += size;

	return size;
}


void netlib::RawPacket::InsertLanHeader()
{
	if (_headerHas == true)
	{
		return;
	}

	_headerHas = true;

	_readPos -= sizeof(LanPacketHeader);
	LanPacketHeader* header = reinterpret_cast<LanPacketHeader*>(GetBufferPtr());
	header->size = GetDataSize() - sizeof(LanPacketHeader);
}

void netlib::RawPacket::Encode(uint8 key)
{
	if (_encoded == true)
	{
		return;
	}

	_encoded = true;

	NetPacketHeader* header = reinterpret_cast<NetPacketHeader*>(_buffer);

	BYTE RK = header->checkSum ^ (header->randKey + 1);
	BYTE KRK = RK ^ (key + 1);
	header->checkSum = KRK;

	for (int32 i = sizeof(NetPacketHeader); i < GetDataSize(); i++)
	{
		RK = _buffer[i] ^ BYTE(header->randKey + RK + i - 3);
		KRK = RK ^ BYTE(key + KRK + i - 3);
		_buffer[i] = KRK;
	}
}

bool netlib::RawPacket::Decode(uint8 key)
{
	NetPacketHeader* header = reinterpret_cast<NetPacketHeader*>(_buffer);


	BYTE prevK = header->checkSum;
	BYTE prevRK = prevK ^ (key + 1);
	header->checkSum = prevRK ^ (header->randKey + 1);

	uint8 checkSum = 0;
	for (int32 i = sizeof(NetPacketHeader); i < GetDataSize(); i++)
	{
		BYTE curK = _buffer[i];
		BYTE curRK = curK ^ BYTE(key + prevK + i - 3);

		_buffer[i] = curRK ^ BYTE(header->randKey + prevRK + i - 3);
		checkSum += _buffer[i];

		prevK = curK;
		prevRK = curRK;
	}



	if (checkSum != header->checkSum)
	{
		return false;
	}

	return true;
	
}

netlib::RawPacket& netlib::RawPacket::operator=(RawPacket& src)
{
	return *this;
}