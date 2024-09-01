#pragma once
#include "Packet.h"
class Player;
class RawPacket;
struct Token;


namespace login
{
	class LoginServer;

	extern bool HandlePacket_LoginServer(LoginServer* server, Player& player, Packet pkt);
	extern Packet Make_S_RES_LOGIN(const int64 accountNo, const bool status, const Token& token);
	extern Packet Make_S_GET_SERVER_LIST(const vector<uint16>& gameServerPorts);
}
