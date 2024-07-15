#pragma once
class Player;
class Packet;
struct Token;
class LoginServer;


 


 
extern bool HandlePacket_LoginServer(LoginServer* server, Player& player, Packet& pkt);
extern Packet& Make_S_RES_LOGIN(const int64 accountNo, const bool status, const Token& token);
extern Packet& Make_S_GET_SERVER_LIST(const vector<wstring>& gameServerIps, const vector<uint16>& gameServerPorts);