#pragma once
#include "Packet.h"
struct Token;



namespace game
{
	class Player;
	class PathReceiver;
	class LoginGroup;
	class LobbyGroup;
	class GameGroup;



	//-------------------------------------------
	/// - uint64 id
	/// - wstring nickname
	/// - int32 level
	/// - int32 modelId
	/// - int32 weaponId
	/// - int32 fieldId
	///
	//-------------------------------------------
	struct Dto_S_GET_CHARACTER_LIST
	{
		uint64 id;
		wstring nickname;
		int32 level;
		int32 modelId;
		int32 weaponId;
		int32 fieldId;
	};



	extern RawPacket& operator<< (RawPacket& pkt, const Dto_S_GET_CHARACTER_LIST& value);
	extern RawPacket& operator>> (RawPacket& pkt, Dto_S_GET_CHARACTER_LIST& value);
	extern bool HandlePacket_LoginGroup(LoginGroup* group, Player& player, Packet pkt);
	extern bool HandlePacket_LobbyGroup(LobbyGroup* group, Player& player, Packet pkt);
	extern bool HandlePacket_GameGroup(GameGroup* group, Player& player, Packet pkt);
	extern Packet Make_S_GAME_LOGIN(const bool status);
	extern Packet Make_S_GET_CHARACTER_LIST(const vector<Dto_S_GET_CHARACTER_LIST>& charaterList);
	extern Packet Make_S_CREATE_CHARACTER(const bool status);
	extern Packet Make_S_GAME_ENTER(const uint64 playerId, const uint64 characterId);
	extern Packet Make_S_SPAWN_CHARACTER(const uint64 playerId, const wstring& nickname, const int32 level, const float32 y, const float32 x, const int32 maxHp, const int32 hp, const int32 maxMp, const int32 mp, const int32 speed, const int32 modelId, const int32 weaponId, const float32 rotation);
	extern Packet Make_S_DESTROY_CHARACTER(const uint64 playerId);
	extern Packet Make_S_MOVE(const uint64 playerId, const PathReceiver& path);
	extern Packet Make_S_SKILL(const uint64 playerId, const uint32 skillId, const float32 targetY, const float32 targetX, const float32 posY, const float32 posX);
	extern Packet Make_S_SPAWN_MONSTER(const uint64 id, const uint64 objectId, const int32 maxHp, const int32 hp, const int32 speed, const float32 y, const float32 x);
	extern Packet Make_S_DESTROY_OBJECT(const uint8 type, const uint64 id);
	extern Packet Make_S_MOVE_OBJECT(const uint8 type, const uint64 id, const PathReceiver& path);
	extern Packet Make_S_DAMAGE(const uint8 type, const uint64 id, const int32 hp);
	extern Packet Make_S_EXP(const int32 curExp);
	extern Packet Make_S_MOVE_FIELD(const uint8 fieldId);
	extern Packet Make_S_MONSTER_ATTACK(const uint64 id, const float32 y, const float32 x, const float32 yaw);
	extern Packet Make_S_LEVEL_UP(const int32 level);
	extern Packet Make_S_CHARACTER_DEATH(const uint64 playerId);
	extern Packet Make_S_CHARACTER_RESPAWN(const uint64 playerId, const int32 hp);
}
