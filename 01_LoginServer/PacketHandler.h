#pragma once
#include "Packet.h"
class RawPacket;
struct Token;

namespace login
{
	class LoginServer;
	class Player;



	extern bool HandlePacket_LoginServer(LoginServer* server, Player& player, Packet pkt);
	extern Packet Make_S_RES_LOGIN(const int64 accountNo, const bool status, const Token& token);
	extern Packet Make_S_GET_SERVER_LIST(const vector<wstring>& gameServerIps, const vector<uint16>& gameServerPorts);
}
