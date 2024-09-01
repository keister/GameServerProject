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
/// @brief TCP 소켓을 생성한다.
/// @return 생성된 TCP 소켓
//-------------------------------------------------------------------------------
SOCKET netlib::Socket::Create()
{
	SOCKET ret = ::socket(AF_INET, SOCK_STREAM, NULL);
	return ret;
}

//-------------------------------------------------------------------------------
/// @brief 소켓을 반환한다.
/// @param socket 반환할 소켓
/// @return 함수 호출 결과
//-------------------------------------------------------------------------------
int32 netlib::Socket::Close(SOCKET socket)
{
	return closesocket(socket);
}

//-------------------------------------------------------------------------------
/// @brief Linger 설정
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
/// @brief L4 수신버퍼 사이즈를 설정한다.
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
/// @brief L4 송신버퍼 사이즈를 설정한다.
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
/// @brief Nagle 알고리즘 on/off 설정한다.
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
