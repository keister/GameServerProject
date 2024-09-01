#pragma once


namespace netlib
{
	class RecvBuffer;

	class RingBuffer
	{
		friend RecvBuffer;
	public:
		RingBuffer();
		RingBuffer(int32 bufferSize);
		~RingBuffer();

		void	Resize(int32 size);

		int32	GetBufferSize();
		int32	GetUseSize();
		int32	GetFreeSize();


		int32	Enqueue(BYTE* data, int32 size);
		int32	Dequeue(BYTE* dest, int32 size);
		int32	Peek(BYTE* dest, int32 size);
		void	Clear();

	private:
		int32	DirectEnqueueSize();
		int32	DirectDequeueSize();
		int32	MoveRear(int32 size);
		int32	MoveFront(int32 size);
		BYTE* GetFrontBufferPtr() { return &_buffer[_front]; }
		BYTE* GetRearBufferPtr() { return &_buffer[_rear]; }
		BYTE* GetBeginBufferPtr() { return _buffer; }

	private:
		BYTE* _buffer;
		int32	_bufferSize;
		int32	_allocSize;
		int32	_front;
		int32	_rear;
	};

}
