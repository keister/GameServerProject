#include "stdafx.h"
#include "Socket.h"


int32 netlib::Socket::Init()
{
	WSADATA wsa;
	return WSAStartup(MAKEWORD(2, 2), &wsa);
}

void netlib::Socket::CleanUp()
{
	WSACleanup();
}

//-------------------------------------------------------------------------------
/// @brief TCP ������ �����Ѵ�.
/// @return ������ TCP ����
//-------------------------------------------------------------------------------
SOCKET netlib::Socket::Create()
{
	SOCKET ret = ::socket(AF_INET, SOCK_STREAM, NULL);
	return ret;
}

//-------------------------------------------------------------------------------
/// @brief ������ ��ȯ�Ѵ�.
/// @param socket ��ȯ�� ����
/// @return �Լ� ȣ�� ���
//-------------------------------------------------------------------------------
int32 netlib::Socket::Close(SOCKET socket)
{
	return closesocket(socket);
}

//-------------------------------------------------------------------------------
/// @brief Linger ����
//-------------------------------------------------------------------------------
bool netlib::Socket::SetLinger(SOCKET socket, uint16 onoff, uint16 linger)
{
	LINGER option{ onoff, linger };
	return SOCKET_ERROR != 
		setsockopt(
			socket, 
			SOL_SOCKET, 
			SO_LINGER, 
			reinterpret_cast<char*>(&option), 
			sizeof(option)
		);
}

//-------------------------------------------------------------------------------
/// @brief L4 ���Ź��� ����� �����Ѵ�.
//-------------------------------------------------------------------------------
bool netlib::Socket::SetRecvBufferSize(SOCKET socket, int32 size)
{
	return SOCKET_ERROR !=
		setsockopt(
			socket,
			SOL_SOCKET,
			SO_RCVBUF,
			reinterpret_cast<char*>(&size),
			sizeof(size)
		);
}

//-------------------------------------------------------------------------------
/// @brief L4 �۽Ź��� ����� �����Ѵ�.
//-------------------------------------------------------------------------------
bool netlib::Socket::SetSendBufferSize(SOCKET socket, int32 size)
{
	return SOCKET_ERROR !=
		setsockopt(
			socket,
			SOL_SOCKET,
			SO_SNDBUF,
			reinterpret_cast<char*>(&size),
			sizeof(size)
		);
}

//-------------------------------------------------------------------------------
/// @brief Nagle �˰��� on/off �����Ѵ�.
//-------------------------------------------------------------------------------
bool netlib::Socket::SetNagle(SOCKET socket, bool flag)
{
	return SOCKET_ERROR !=
		setsockopt(
			socket,
			SOL_SOCKET,
			TCP_NODELAY,
			reinterpret_cast<char*>(&flag),
			sizeof(flag)
		);
}

bool netlib::Socket::Bind(SOCKET socket, NetworkAddress addr)
{
	SOCKADDR_IN sockAddr = addr.GetSockAddr();
	return SOCKET_ERROR !=
		::bind(socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(addr));
}

bool netlib::Socket::Listen(SOCKET socket, int32 backlog)
{
	return SOCKET_ERROR !=
		::listen(socket, backlog);
}

bool netlib::Socket::Connect(SOCKET socket, NetworkAddress addr)
{
	SOCKADDR_IN sockAddr = addr.GetSockAddr();
	return SOCKET_ERROR !=
		::connect(socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(addr));
}
