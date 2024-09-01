using System.Collections.Concurrent;
using System.Collections.Generic;
using System;
using System.Net;
using System.Net.Sockets;
using Controller.Network;
using System.Threading;
using UnityEngine;

public abstract class NetClient : MonoBehaviour
{
    private Socket _socket;
    private RingBuffer _sendBuffer;
    private RingBuffer _recvBuffer;
    private SocketAsyncEventArgs _recvEventArgs;
    private SocketAsyncEventArgs _sendEventArgs;
    protected Byte _packetCode;
    protected Byte _encodingKey;
    private ConcurrentQueue<Packet> _packetQueue;

    private bool canProcessPacket = true;
    private Int32 _isSendRestricted = 0;

    private Dictionary<int, Action<Packet>> PacketDict = new();

    public void Bind(UInt16 idx, Action<Packet> callback)
    {
        PacketDict.Add(idx, callback);
    }

    public void Unbind(UInt16 idx)
    {
        PacketDict.Remove(idx);
    }
    
    public NetClient()
    {
        _packetQueue = new ConcurrentQueue<Packet>();
        _socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
        _recvBuffer = new RingBuffer();
        _sendBuffer = new RingBuffer();
        
        _recvEventArgs = new SocketAsyncEventArgs();
        _recvEventArgs.UserToken = _socket;
        _recvEventArgs.Completed += RecvCompletionCallback;
        
        _sendEventArgs = new SocketAsyncEventArgs();
        _sendEventArgs.UserToken = _socket;
        _sendEventArgs.Completed += SendCompletionCallback;
    }

    public void Update()
    {
        while (DequeuePacket(out Packet pkt))
        {
            pkt.Read(out UInt16 type);
            
            if (PacketDict.TryGetValue(type, out Action<Packet> act))
            {
                act(pkt);
            }
            else
            {
                Debug.Log($"cant get {type}");
            }

            if (canProcessPacket == false)
            {
                break;
            }
        }
    }

    public void PausePacket()
    {
        canProcessPacket = false;
    }

    public void ResumePacket()
    {
        canProcessPacket = true;
    }
    
    public void EnqueuePacket(Packet pkt)
    {
        _packetQueue.Enqueue(pkt);
    }

    public bool DequeuePacket(out Packet pkt)
    {
        return _packetQueue.TryDequeue(out pkt);
    }

    public bool Connect(string host, Int32 port)
    {
        IPAddress ip;
        
        if (host == "localhost")
        {
            ip = IPAddress.Loopback;
        }
        else
        {
            ip = Dns.GetHostAddresses(host)[0];
        }

        try
        {
            _socket.Connect(ip, port);
        }
        catch (Exception e)
        {
            Debug.Log($"{e}");
            return false;
        }
        OnConnect();
        RecvPost();
        return true;
    }
    

    public void Disconnect()
    {
        _socket.Close();
        OnDisconnect();
    }


    public void SendPacket(Packet pkt)
    {
        pkt.InsertNetHeader(_packetCode);
        pkt.Encode(_encodingKey);
        _sendBuffer.Enqueue(pkt.GetBufferSegment(), pkt.GetDataSize());
        
        SendPost();
    }

    private void RecvPost()
    {
        
        _recvEventArgs.BufferList = _recvBuffer.GetFreeSegments();
        bool pending;
        try
        {
            pending = _socket.ReceiveAsync(_recvEventArgs);
        }
        catch (Exception e)
        {
            Debug.Log($"{e}");
            return;
        }

        if (!pending)
        {
            RecvCompletionCallback(null, _recvEventArgs);
        }
    }


    private bool SendPost()
    {
        if (Interlocked.Exchange(ref _isSendRestricted, 1) == 1)
        {
            return false;
        }
        
        if (_sendBuffer.GetUseSize() == 0)
        {
            Interlocked.Exchange(ref _isSendRestricted, 0);
            return false;
        }
        _sendEventArgs.BufferList = _sendBuffer.GetUseSegments();
        bool pending;
        try
        {
            pending = _socket.SendAsync(_sendEventArgs);
        }
        catch (Exception e)
        {
            Debug.Log($"{e}");
            return false;
        }
        
        if (!pending)
        {
            SendCompletionCallback(null, _recvEventArgs);
        }

        return true;
    }
    private void RecvCompletionCallback(System.Object sender, SocketAsyncEventArgs args)
    {
        _recvBuffer.MoveRear(args.BytesTransferred);

        while (true)
        {
            if (_recvBuffer.GetUseSize() < Packet.NetPacketHeader.SIZE)
            {
                break;
            }
            ArraySegment<byte> headerSeg;
            Int32 ret = _recvBuffer.Peek(out headerSeg, Packet.NetPacketHeader.SIZE);
            
            UInt16 size =  BitConverter.ToUInt16(headerSeg.Array, 1);;
            if (_recvBuffer.GetUseSize() < size)
            {
                break;
            }

            Packet pkt = new Packet();
            pkt.MoveReadPos(-Packet.NetPacketHeader.SIZE);
            
            _recvBuffer.Dequeue( pkt.GetBufferSegment(), size + Packet.NetPacketHeader.SIZE);
            pkt.MoveWritePos(size);
            pkt.Decode(_encodingKey);
            pkt.MoveReadPos(Packet.NetPacketHeader.SIZE);
            EnqueuePacket(pkt);
        }
        
        RecvPost();
        return;
    }
    
    private void SendCompletionCallback(System.Object sender, SocketAsyncEventArgs args)
    {
        _sendBuffer.MoveFront(args.BytesTransferred);
        Interlocked.Exchange(ref _isSendRestricted, 0);
        
        if (_sendBuffer.GetUseSize() >= 0)
        {
            SendPost();
        }
    }
    
    public abstract void OnConnect();
    public abstract void OnDisconnect();
}
