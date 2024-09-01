#pragma once
#include "Packet.h"
class Player;
class RawPacket;
struct Token;


namespace chat
{
	class ChatServer;

	extern bool HandlePacket_ChatServer(ChatServer* server, Player& player, netlib::Packet pkt);
	extern netlib::Packet Make_S_CHAT_LOGIN(const bool status);
	extern netlib::Packet Make_S_CHAT_ENTER(const bool status);
	extern netlib::Packet Make_S_CHAT(const uint8 chatType, const uint64 playerId, const uint64 characterId, const wstring& nickname, const wstring& message);
	extern netlib::Packet Make_S_WHISPER(const uint64 playerId, const uint64 characterId, const wstring& nickname, const wstring& message);
	extern netlib::Packet Make_S_CHAT_FAIL(const int32 code);
}
