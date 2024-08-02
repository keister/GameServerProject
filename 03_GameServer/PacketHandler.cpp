#include "stdafx.h"
#include "PacketHandler.h"

#include "Player.h"
#include "Packet.h"
#include "LoginGroup.h"
#include "LobbyGroup.h"
#include "GameGroup_Town.h"


#include "Common/PacketDefine.h"
#include "Common/Token.h"
RawPacket& operator<< (RawPacket& pkt, const Dto_S_GET_CHARACTER_LIST& value)
{

	pkt << value.id << value.nickname << value.level << value.modelId << value.weaponId;

	return pkt;
}
RawPacket& operator>> (RawPacket& pkt, Dto_S_GET_CHARACTER_LIST& value)
{

	pkt << value.id >> value.nickname >> value.level >> value.modelId >> value.weaponId;

	return pkt;
}
bool HandlePacket_LoginGroup(LoginGroup* group, Player& player, Packet pkt)
{
	PacketType type;
	*pkt >> type;

	switch (type)
	{
	case PacketType::C_GAME_LOGIN:
	{
		uint64 accountId;
		Token token;
		*pkt >> accountId >> token;

		group->Handle_C_GAME_LOGIN(player, accountId, token);
		break;
	}
	default:
		return false;
	}

	return true;
}
bool HandlePacket_LobbyGroup(LobbyGroup* group, Player& player, Packet pkt)
{
	PacketType type;
	*pkt >> type;

	switch (type)
	{
	case PacketType::C_GET_CHARACTER_LIST:
	{

		group->Handle_C_GET_CHARACTER_LIST(player);
		break;
	}
	case PacketType::C_CREATE_CHARACTER:
	{
		wstring nickname;
		int32 modelId;
		int32 weaponId;
		*pkt >> nickname >> modelId >> weaponId;

		group->Handle_C_CREATE_CHARACTER(player, nickname, modelId, weaponId);
		break;
	}
	case PacketType::C_GAME_ENTER:
	{
		uint64 characterId;
		int32 idx;
		*pkt >> characterId >> idx;

		group->Handle_C_GAME_ENTER(player, characterId, idx);
		break;
	}
	default:
		return false;
	}

	return true;
}
bool HandlePacket_GameGroup_Town(GameGroup_Town* group, Player& player, Packet pkt)
{
	PacketType type;
	*pkt >> type;

	switch (type)
	{
	case PacketType::C_MOVE:
	{
		float32 y;
		float32 x;
		*pkt >> y >> x;

		group->Handle_C_MOVE(player, y, x);
		break;
	}
	case PacketType::C_SKILL:
	{
		uint64 objectId;
		uint32 skillId;
		*pkt >> objectId >> skillId;

		group->Handle_C_SKILL(player, objectId, skillId);
		break;
	}
	default:
		return false;
	}

	return true;
}
Packet Make_S_GAME_LOGIN(const bool status)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_GAME_LOGIN;
	*pkt << status;

	return pkt;
}
Packet Make_S_GET_CHARACTER_LIST(const vector<Dto_S_GET_CHARACTER_LIST>& charaterList)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_GET_CHARACTER_LIST;
	*pkt << charaterList;

	return pkt;
}
Packet Make_S_CREATE_CHARACTER(const bool status)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_CREATE_CHARACTER;
	*pkt << status;

	return pkt;
}
Packet Make_S_GAME_ENTER(const uint64 characterId, const wstring& nickname, const int32 level, const int32 exp, const float32 y, const float32 x, const int32 hp, const int32 speed, const int32 fieldId)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_GAME_ENTER;
	*pkt << characterId << nickname << level << exp << y << x << hp << speed << fieldId;

	return pkt;
}
Packet Make_S_SPAWN_CHARACTER(const uint64 playerId, const wstring& nickname, const int32 level, const float32 y, const float32 x, const int32 hp, const int32 speed, const int32 modelId, const int32 weaponId, const float32 rotation)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_SPAWN_CHARACTER;
	*pkt << playerId << nickname << level << y << x << hp << speed << modelId << weaponId << rotation;

	return pkt;
}
Packet Make_S_DESTROY_CHARACTER(const uint64 playerId)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_DESTROY_CHARACTER;
	*pkt << playerId;

	return pkt;
}
Packet Make_S_MOVE(const float32 y, const float32 x)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_MOVE;
	*pkt << y << x;

	return pkt;
}
Packet Make_S_MOVE_OTHER(const uint64 playerId, const float32 y, const float32 x)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_MOVE_OTHER;
	*pkt << playerId << y << x;

	return pkt;
}
Packet Make_S_SKILL(const uint32 skillId, const float32 targetY, const float32 targetX, const float32 posY, const float32 posX)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_SKILL;
	*pkt << skillId << targetY << targetX << posY << posX;

	return pkt;
}
Packet Make_S_SKILL_OTHER(const uint64 playerId, const uint32 skillId, const float32 targetY, const float32 targetX, const float32 posY, const float32 posX)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_SKILL_OTHER;
	*pkt << playerId << skillId << targetY << targetX << posY << posX;

	return pkt;
}
Packet Make_S_SPAWN_MONSTER(const uint64 id, const uint64 objectId, const int32 hp, const int32 speed, const float32 y, const float32 x)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_SPAWN_MONSTER;
	*pkt << id << objectId << hp << speed << y << x;

	return pkt;
}
Packet Make_S_DESTROY_OBJECT(const uint8 type, const uint64 id)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_DESTROY_OBJECT;
	*pkt << type << id;

	return pkt;
}
Packet Make_S_MOVE_OBJECT(const uint8 type, const uint64 id, const float32 y, const float32 x)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_MOVE_OBJECT;
	*pkt << type << id << y << x;

	return pkt;
}
Packet Make_S_DAMAGE(const uint64 id, const int32 hp)
{
	Packet pkt = Packet::Alloc();

	*pkt << PacketType::S_DAMAGE;
	*pkt << id << hp;

	return pkt;
}