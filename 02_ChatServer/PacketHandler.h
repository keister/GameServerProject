#pragma once
#include "Packet.h"
class Player;
class RawPacket;
struct Token;
class ChatServer;


 


 
extern bool HandlePacket_ChatServer(ChatServer* server, Player& player, Packet pkt);
extern Packet Make_S_CHAT_LOGIN(const bool status);
extern Packet Make_S_CHAT_ENTER(const bool status);
extern Packet Make_S_CHAT(const uint8 chatType, const uint64 playerId, const uint64 characterId, const wstring& nickname, const wstring& message);
extern Packet Make_S_WHISPER(const uint64 playerId, const uint64 characterId, const wstring& nickname, const wstring& message);
extern Packet Make_S_CHAT_FAIL(const int32 code);