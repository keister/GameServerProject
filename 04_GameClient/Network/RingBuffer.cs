using System;
using System.Collections.Generic;
using Unity.VisualScripting;
using UnityEngine;

namespace Controller.Network
{
    public class RingBuffer
    {
        private Byte[] _buffer;
        public Int32 bufferSize { get; private set; }
        public Int32 allocSize { get; private set; }
        private Int32 _front;
        private Int32 _rear;

        public RingBuffer()
        {
            bufferSize = 10000;
            allocSize = bufferSize + 1;
            _front = 0;
            _rear = 0;
            _buffer = new byte[allocSize];
        }

        public void MoveFront(Int32 size)
        {
            _front = (_front + size) % allocSize;
        }
        
        public void MoveRear(Int32 size)
        {
            _rear = (_rear + size) % allocSize;
        }
        
        public Int32 GetUseSize()
        {
            Int32 nowRear = _rear;
            Int32 nowFront = _front;

            if (nowRear >= nowFront)
            {
                return nowRear - nowFront;
            }
            else
            {
                return bufferSize + nowRear - nowFront + 1;
            }
        }

        public Int32 GetFreeSize()
        {
            Int32 nowRear = _rear;
            Int32 nowFront = _front;

            if (nowRear >= nowFront)
            {
                return bufferSize - nowRear + nowFront;
            }
            else
            {
                return nowFront - nowRear - 1;
            }
        }

        public Int32 DirectEnqueueSize()
        {
            Int32 nowRear = _rear;
            Int32 nowFront = _front;

            if (nowRear >= nowFront)
            {
                if (nowFront == 0)
                {
                    return bufferSize - nowRear;
                }
                else
                {
                    return bufferSize - nowRear + 1;
                }
            }
            else
            {
                return nowFront - nowRear - 1;
            }
        }

        public Int32 DirectDequeueSize()
        {
            Int32 nowRear = _rear;
            Int32 nowFront = _front;

            if (nowRear >= nowFront)
            {
                return nowRear - nowFront;
            }
            else
            {
                return allocSize - nowFront;
            }
        }

        public Int32 Enqueue(ArraySegment<Byte> data, Int32 size)
        {
            if (size > GetFreeSize())
            {
                return -1;
            }

            if (_rear + size > bufferSize)
            {
                int chunkSize = allocSize - _rear;
                Array.Copy(data.Array, 0, _buffer, _rear, chunkSize);
                Array.Copy(data.Array, chunkSize, _buffer, 0, size - chunkSize);
            }
            else
            {
                Array.Copy(data.Array, 0, _buffer, _rear, size);
            }

            _rear = (_rear + size) % allocSize;

            return size;
        }

        public Int32 Dequeue(ArraySegment<Byte> dest, Int32 size)
        {
            if (size > GetUseSize())
            {
                return -1;
            }
            
            if (_front + size > bufferSize)
            {
                int chunkSize = allocSize - _front;
                Array.Copy(_buffer, _front, dest.Array, 0, chunkSize);
                Array.Copy(_buffer, 0, dest.Array, chunkSize, size - chunkSize);
            }
            else
            {
                Array.Copy(_buffer, _front, dest.Array, 0, size);
            }
            
            _front = (_front + size) % allocSize;
            return size;
        }

        public Int32 Peek(out ArraySegment<Byte> dest, Int32 size)
        {
            if (size > GetUseSize())
            {
                dest = null;
                return -1;
            }

            byte[] destArray = new byte[size];
            
            if (_front + size > bufferSize)
            {
                int chunkSize = allocSize - _front;
                Array.Copy(_buffer, _front, destArray, 0, chunkSize);
                Array.Copy(_buffer, 0, destArray, chunkSize, size - chunkSize);
            }
            else
            {
                Array.Copy(_buffer, _front, destArray, 0, size);
            }

            dest = new ArraySegment<byte>(destArray);
            
            return size;
        }
        
        public List<ArraySegment<Byte>> GetUseSegments()
        {
            List<ArraySegment<Byte>> wsabufs = new List<ArraySegment<byte>>();
            Int32 directSize = DirectDequeueSize();
            wsabufs.Add(new ArraySegment<Byte>(_buffer, _front, directSize));
            Int32 remainSize = GetUseSize() - directSize;
            if (directSize > 0)
            {
                wsabufs.Add(new ArraySegment<Byte>(_buffer, 0, remainSize));
            }
            return wsabufs;
        }
        
        public List<ArraySegment<Byte>> GetFreeSegments()
        {
            List<ArraySegment<Byte>> wsabufs = new List<ArraySegment<byte>>();
            Int32 directSize = DirectEnqueueSize();
            wsabufs.Add(new ArraySegment<Byte>(_buffer, _rear, directSize));

            Int32 remainSize = GetFreeSize() -directSize;
            if (directSize > 0)
            {
                wsabufs.Add(new ArraySegment<Byte>(_buffer, 0, remainSize));
            }
            return wsabufs;
        }
    }
}