#include "stdafx.h"
#include "PacketHandler.h"

#include "Player.h"
#include "Packet.h"
#include "ChatServer.h"


#include "Common/PacketDefine.h"
#include "Common/Token.h"


namespace chat
{
	bool HandlePacket_ChatServer(ChatServer* server, Player& player, Packet pkt)
	{
		PacketType type;
		*pkt >> type;

		switch (type)
		{
		case PacketType::C_CHAT_LOGIN:
		{
			uint64 accountId;
			Token token;
			*pkt >> accountId >> token;

			server->Handle_C_CHAT_LOGIN(player, accountId, token);
			break;
		}
		case PacketType::C_CHAT_ENTER:
		{
			uint64 characterId;
			*pkt >> characterId;

			server->Handle_C_CHAT_ENTER(player, characterId);
			break;
		}
		case PacketType::C_CHAT_LEAVE:
		{

			server->Handle_C_CHAT_LEAVE(player);
			break;
		}
		case PacketType::C_CHAT_MOVE_FIELD:
		{
			int32 fieldId;
			*pkt >> fieldId;

			server->Handle_C_CHAT_MOVE_FIELD(player, fieldId);
			break;
		}
		case PacketType::C_CHAT:
		{
			uint8 chatType;
			wstring message;
			*pkt >> chatType >> message;

			server->Handle_C_CHAT(player, chatType, message);
			break;
		}
		case PacketType::C_WHISPER:
		{
			wstring toNickname;
			wstring message;
			*pkt >> toNickname >> message;

			server->Handle_C_WHISPER(player, toNickname, message);
			break;
		}
		default:
			return false;
		}

		return true;
	}
	Packet Make_S_CHAT_LOGIN(const bool status)
	{
		Packet pkt = Packet::Alloc();

		*pkt << PacketType::S_CHAT_LOGIN;
		*pkt << status;

		return pkt;
	}
	Packet Make_S_CHAT_ENTER(const bool status)
	{
		Packet pkt = Packet::Alloc();

		*pkt << PacketType::S_CHAT_ENTER;
		*pkt << status;

		return pkt;
	}
	Packet Make_S_CHAT(const uint8 chatType, const uint64 playerId, const uint64 characterId, const wstring& nickname, const wstring& message)
	{
		Packet pkt = Packet::Alloc();

		*pkt << PacketType::S_CHAT;
		*pkt << chatType << playerId << characterId << nickname << message;

		return pkt;
	}
	Packet Make_S_WHISPER(const uint64 playerId, const uint64 characterId, const wstring& nickname, const wstring& message)
	{
		Packet pkt = Packet::Alloc();

		*pkt << PacketType::S_WHISPER;
		*pkt << playerId << characterId << nickname << message;

		return pkt;
	}
	Packet Make_S_CHAT_FAIL(const int32 code)
	{
		Packet pkt = Packet::Alloc();

		*pkt << PacketType::S_CHAT_FAIL;
		*pkt << code;

		return pkt;
	}

}
