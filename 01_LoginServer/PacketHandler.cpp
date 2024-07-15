#include "stdafx.h"
#include "PacketHandler.h"

#include "Player.h"
#include "Packet.h"
#include "LoginServer.h"


#include "Common/PacketDefine.h"
#include "Common/Token.h"
bool HandlePacket_LoginServer(LoginServer* server, Player& player, Packet& pkt)
{
	PacketType type;
	pkt >> type;

	switch (type)
	{
	case PacketType::C_REQ_LOGIN:
	{
		int64 accountNo;
		Token token;
		pkt >> accountNo >> token;

		server->Handle_C_REQ_LOGIN(player,accountNo, token);
		break;
	}
	case PacketType::C_GET_SERVER_LIST:
	{

		server->Handle_C_GET_SERVER_LIST(player);
		break;
	}
	default:
		return false;
	}

	return true;
}
Packet& Make_S_RES_LOGIN(const int64 accountNo, const bool status, const Token& token)
{
	Packet& pkt = Packet::Alloc();

	pkt << PacketType::S_RES_LOGIN;
	pkt << accountNo << status << token;

	return pkt;
}
Packet& Make_S_GET_SERVER_LIST(const vector<wstring>& gameServerIps, const vector<uint16>& gameServerPorts)
{
	Packet& pkt = Packet::Alloc();

	pkt << PacketType::S_GET_SERVER_LIST;
	pkt << gameServerIps << gameServerPorts;

	return pkt;
}