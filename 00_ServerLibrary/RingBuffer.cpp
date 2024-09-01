#include "stdafx.h"
#include "RingBuffer.h"

netlib::RingBuffer::RingBuffer()
{
	_bufferSize = 10000;
	_allocSize = _bufferSize + 1;
	_front = 0;
	_rear = 0;

	_buffer = new BYTE[_allocSize];
	::memset(_buffer, 0, _allocSize);
}

netlib::RingBuffer::RingBuffer(int32 bufferSize)
{
	_bufferSize = bufferSize;
	_allocSize = _bufferSize + 1;
	_front = 0;
	_rear = 0;

	_buffer = new BYTE[_allocSize];
	::memset(_buffer, 0, _allocSize);
}

netlib::RingBuffer::~RingBuffer()
{
	delete _buffer;
}

void netlib::RingBuffer::Resize(int32 size)
{
	int32 useSize = GetUseSize();
	BYTE* newBuffer = new BYTE[size];
	::memcpy(newBuffer, &_buffer[_front], useSize);
	delete _buffer;
	_buffer = newBuffer;
	_allocSize = size + 1;
	_bufferSize = size;
	_front = 0;
	_rear = useSize;
}

int32 netlib::RingBuffer::GetBufferSize()
{
	return _bufferSize;
}

int32 netlib::RingBuffer::GetUseSize()
{
	int32 rear = _rear;
	int32 front = _front;

	if (rear >= front)
	{
		return rear - front;
	}
	else
	{
		return _bufferSize + rear - front + 1;
	}
}

int32 netlib::RingBuffer::GetFreeSize()
{
	int32 rear = _rear;
	int32 front = _front;

	if (rear >= front)
	{
		return _bufferSize - rear + front;
	}
	else
	{
		return front - rear - 1;
	}
}

int32 netlib::RingBuffer::Enqueue(BYTE* data, int size)
{
	if (size > GetFreeSize())
	{
		return -1;
	}

	if (_rear + size > _bufferSize)
	{
		int32 chunkSize = _allocSize - _rear;
		::memcpy(&_buffer[_rear], data, chunkSize);
		data += chunkSize;
		::memcpy(&_buffer[0], data, size - chunkSize);
	}
	else
	{
		::memcpy(&_buffer[_rear], data, size);
	}

	_rear = (_rear + size) % _allocSize;

	return size;
}

int32 netlib::RingBuffer::Dequeue(BYTE* dest, int size)
{
	if (size > GetUseSize())
		return -1;

	if (_front + size > _bufferSize)
	{
		int32 chunkSize = _allocSize - _front;
		::memcpy(dest, &_buffer[_front], chunkSize);
		dest += chunkSize;
		::memcpy(dest, &_buffer[0], size - chunkSize);
	}
	else
	{
		::memcpy(dest, &_buffer[_front], size);
	}

	_front = (_front + size) % _allocSize;


	return size;
}

int32 netlib::RingBuffer::Peek(BYTE* dest, int size)
{
	if (size > GetUseSize())
		return -1;

	if (_front + size > _bufferSize)
	{
		int32 chunkSize = _allocSize - _front;
		::memcpy(dest, &_buffer[_front], chunkSize);
		dest += chunkSize;
		::memcpy(dest, &_buffer[0], size - chunkSize);
	}
	else
	{
		::memcpy(dest, &_buffer[_front], size);
	}


	return size;
}

int32 netlib::RingBuffer::DirectEnqueueSize()
{

	int32 rear = _rear;
	int32 front = _front;

	if (rear >= front)
	{
		if (front == 0)
		{
			return _bufferSize - rear;
		}
		else
		{
			return _bufferSize - rear + 1;
		}
	}
	else
	{
		return front - rear - 1;
	}
}

int32 netlib::RingBuffer::DirectDequeueSize()
{
	int32 rear = _rear;
	int32 front = _front;

	if (rear >= front)
	{
		return rear - front;
	}
	else
	{
		return _allocSize - front;
	}
}

int32 netlib::RingBuffer::MoveRear(int32 size)
{
	_rear = (_rear + size) % _allocSize;
	return size;
}

int32 netlib::RingBuffer::MoveFront(int32 size)
{
	_front = (_front + size) % _allocSize;
	return size;
}

void netlib::RingBuffer::Clear()
{
	_front = 0;
	_rear = 0;
}
