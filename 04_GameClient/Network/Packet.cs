// 2024062817:06
// 2024062817:06
// 김호준rkaghMy projectAssembly-CSharpPacket.cs


using System;
using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using UnityEngine;
using Object = UnityEngine.Object;
using Random = System.Random;

namespace Controller.Network
{
    public class Packet
    {
        private static Dictionary<Type, Func<Packet, object>> readFunc = new();
        private static Dictionary<Type, Func<Packet, object>> writeFunc = new();

        static Packet()
        {
            readFunc[typeof(Char)] = (pkt) => ReadChar(pkt);
            readFunc[typeof(bool)] = (pkt) => ReadBool(pkt);
            readFunc[typeof(Byte)] = (pkt) => ReadByte(pkt);
            readFunc[typeof(byte[])] = (pkt) => ReadByteArray(pkt);
            readFunc[typeof(Int16)] = (pkt) => ReadInt16(pkt);
            readFunc[typeof(UInt16)] = (pkt) => ReadUInt16(pkt);
            readFunc[typeof(Int32)] = (pkt) => ReadInt32(pkt);
            readFunc[typeof(UInt32)] = (pkt) => ReadUInt32(pkt);
            readFunc[typeof(Int64)] = (pkt) => ReadInt64(pkt);
            readFunc[typeof(UInt64)] = (pkt) => ReadUInt64(pkt);
            readFunc[typeof(Single)] = (pkt) => ReadSingle(pkt);
            readFunc[typeof(Double)] = (pkt) => ReadDouble(pkt);
            readFunc[typeof(string)] = (pkt) => ReadString(pkt);
        }

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public struct NetPacketHeader
        {
            public static readonly Int32 SIZE = Marshal.SizeOf<NetPacketHeader>();
            public byte code;
            public UInt16 size;
            public byte randKey;
            public byte checkSum;
        }

        private static readonly Int32 BUFFER_DEFAULT_SIZE = 500;

        private byte[] buffer;
        private Int32 bufferSize;
        private Int32 readPos;
        private Int32 writePos;
        private bool encoded;
        private bool headerHas;


        public Packet()
        {
            bufferSize = BUFFER_DEFAULT_SIZE + NetPacketHeader.SIZE;
            buffer = new byte[bufferSize];
            readPos = NetPacketHeader.SIZE;
            writePos = NetPacketHeader.SIZE;
        }

        public Int32 MoveWritePos(Int32 size)
        {
            writePos += size;
            return size;
        }

        public Int32 MoveReadPos(Int32 size)
        {
            readPos += size;
            return size;
        }

        public Int32 GetBufferSize()
        {
            return bufferSize;
        }

        public Int32 GetDataSize()
        {
            return writePos - readPos;
        }

        public void InsertNetHeader(Byte packetCode)
        {
            if (headerHas)
            {
                return;
            }

            headerHas = true;
            readPos -= NetPacketHeader.SIZE;

            NetPacketHeader header = new NetPacketHeader();
            header.size = (UInt16)(GetDataSize() - NetPacketHeader.SIZE);
            header.code = packetCode;
            header.randKey = (byte)new Random().Next(0, 255);
            // header.randKey = (byte)0x29;
            buffer[0] = header.code;
            Array.Copy(BitConverter.GetBytes(header.size), 0, buffer, 1, 2);
            buffer[3] = header.randKey;
        }


        public ArraySegment<Byte> GetBufferSegment()
        {
            return new ArraySegment<Byte>(buffer);
        }

        #region ##Encoding

        public void Encode(Byte key)
        {
            if (encoded)
            {
                return;
            }

            encoded = true;

            ref Byte checkSum = ref buffer[4];
            Byte randKey = buffer[3];
            Int32 dataSize = GetDataSize();
            for (Int32 i = NetPacketHeader.SIZE; i < dataSize; i++)
            {
                checkSum += buffer[i];
            }

            Byte rk = (Byte)(checkSum ^ (randKey + 1));
            Byte krk = (Byte)(rk ^ (key + 1));
            checkSum = krk;

            for (Int32 i = NetPacketHeader.SIZE; i < dataSize; i++)
            {
                rk = (Byte)(buffer[i] ^ (Byte)(randKey + rk + i - 3));
                krk = (Byte)(rk ^ (Byte)(key + krk + i - 3));
                buffer[i] = krk;
            }

            buffer[4] = checkSum;
        }

        public bool Decode(Byte key)
        {
            ref Byte checkSum = ref buffer[4];
            Byte randKey = buffer[3];
            Byte prevK = checkSum;
            Byte prevRk = (Byte)(prevK ^ (key + 1));
            checkSum = (Byte)(prevRk ^ (randKey + 1));

            Byte nowCheckSum = 0;
            for (Int32 i = NetPacketHeader.SIZE; i < GetDataSize(); i++)
            {
                Byte curK = buffer[i];
                Byte curRk = (Byte)(curK ^ (key + prevK + i - 3));

                buffer[i] = (Byte)(curRk ^ (randKey + prevRk + i - 3));
                nowCheckSum += buffer[i];

                prevK = curK;
                prevRk = curRk;
            }

            if (nowCheckSum != checkSum)
            {
                return false;
            }

            return true;
        }

        #endregion

        #region ##Read

        public static Char ReadChar(Packet pkt)
        {
            var ret = BitConverter.ToChar(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(Char);
            return ret;
        }

        public static bool ReadBool(Packet pkt)
        {
            var ret = BitConverter.ToBoolean(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(bool);
            return ret;
        }

        public static Byte ReadByte(Packet pkt)
        {
            var ret = pkt.buffer[pkt.readPos];
            pkt.readPos += sizeof(Byte);
            return ret;
        }

        public static byte[] ReadByteArray(Packet pkt)
        {
            pkt.Read(out UInt16 size);
            byte[] ret = new byte[size];

            Array.Copy(pkt.buffer, pkt.readPos, ret, 0, size);
            pkt.readPos += size;
            return ret;
        }

        public static Int16 ReadInt16(Packet pkt)
        {
            var ret = BitConverter.ToInt16(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(Int16);
            return ret;
        }

        public static UInt16 ReadUInt16(Packet pkt)
        {
            var ret = BitConverter.ToUInt16(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(UInt16);
            return ret;
        }

        public static Int32 ReadInt32(Packet pkt)
        {
            var ret = BitConverter.ToInt32(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(Int32);
            return ret;
        }

        public static UInt32 ReadUInt32(Packet pkt)
        {
            var ret = BitConverter.ToUInt32(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(UInt32);
            return ret;
        }

        public static Int64 ReadInt64(Packet pkt)
        {
            var ret = BitConverter.ToInt64(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(Int64);
            return ret;
        }

        public static UInt64 ReadUInt64(Packet pkt)
        {
            var ret = BitConverter.ToUInt64(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(UInt64);
            return ret;
        }

        public static Single ReadSingle(Packet pkt)
        {
            var ret = BitConverter.ToSingle(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(Single);
            return ret;
        }

        public static Double ReadDouble(Packet pkt)
        {
            var ret = BitConverter.ToDouble(pkt.buffer, pkt.readPos);
            pkt.readPos += sizeof(Double);
            return ret;
        }

        public static string ReadString(Packet pkt)
        {
            pkt.Read(out UInt16 size);
            var ret = Encoding.Unicode.GetString(pkt.buffer, pkt.readPos, size);
            pkt.readPos += size;
            return ret;
        }

        public Packet Read<T>(out T dest)
        {
            if (readFunc.TryGetValue(typeof(T), out var func))
            {
                dest = (T)func(this);
            }
            else
            {
                throw new NotSupportedException();
            }

            return this;
        }

        public Packet Read<T>(out List<T> dest)
        {
            dest = new List<T>();
            Read(out UInt16 size);

            for (Int32 i = 0; i < size; i++)
            {
                Read(out T item);
                dest.Add(item);
            }

            return this;
        }

        #endregion

        #region ##Write

        public void Write(Byte value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Byte));
            writePos += sizeof(Byte);
        }

        public void Write(Byte[] value)
        {
            Write((UInt16)value.Length);
            Array.Copy(value, 0, buffer, writePos, value.Length);
            writePos += value.Length;
        }

        public void Write(Char value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Char));
            writePos += sizeof(Char);
        }

        public void Write(Int16 value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Int16));
            writePos += sizeof(Int16);
        }

        public void Write(UInt16 value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(UInt16));
            writePos += sizeof(UInt16);
        }

        public void Write(Int32 value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Int32));
            writePos += sizeof(Int32);
        }

        public void Write(UInt32 value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(UInt32));
            writePos += sizeof(UInt32);
        }

        public void Write(Int64 value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Int64));
            writePos += sizeof(Int64);
        }

        public void Write(UInt64 value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(UInt64));
            writePos += sizeof(UInt64);
        }

        public void Write(Single value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Single));
            writePos += sizeof(Single);
        }

        public void Write(Double value)
        {
            byte[] val = BitConverter.GetBytes(value);
            Array.Copy(val, 0, buffer, writePos, sizeof(Double));
            writePos += sizeof(Double);
        }


        public void Write(String value)
        {
            UInt16 size = (UInt16)(value.Length * 2);
            Write(size);
            Array.Copy(Encoding.Unicode.GetBytes(value), 0, buffer, writePos, size);
            writePos += size;
        }

        #endregion
    }
}