#include "stdafx.h"
#include "NetworkAddress.h"

netlib::NetworkAddress::NetworkAddress(SOCKADDR_IN sockAddr) : _sockAddr(sockAddr)
{
}

netlib::NetworkAddress::NetworkAddress(const wstring_view ip, uint16 port)
{
	_sockAddr = {};
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr = Ip2Address(ip.data());
	_sockAddr.sin_port = ::htons(port);
}

netlib::NetworkAddress::NetworkAddress(uint32 ipType, uint16 port)
{
	_sockAddr = {};
	_sockAddr.sin_family = AF_INET;
	_sockAddr.sin_addr.s_addr = ::htonl(ipType);
	_sockAddr.sin_port = ::htons(port);
}

wstring netlib::NetworkAddress::GetIpAddress() const
{
	WCHAR buf[100];
	::InetNtopW(AF_INET, &_sockAddr.sin_addr, buf, 100);
	return buf;
}

IN_ADDR netlib::NetworkAddress::Ip2Address(const wstring_view ip)
{
	IN_ADDR address;
	::InetPton(AF_INET, ip.data(), &address);
	return address;
}
