#pragma once
class Player;
class Packet;
struct Token;
class LoginGroup;
class LobbyGroup;
class GameGroup_Town;


 
//-------------------------------------------
/// - uint64 id
/// - wstring nickname
/// - int32 level
/// - int32 modelId
/// - int32 weaponId
///
//-------------------------------------------
struct Dto_S_GET_CHARACTER_LIST
{
	uint64 id;
	wstring nickname;
	int32 level;
	int32 modelId;
	int32 weaponId;
};


 
extern Packet& operator<< (Packet& pkt, const Dto_S_GET_CHARACTER_LIST& value);
extern Packet& operator>> (Packet& pkt, Dto_S_GET_CHARACTER_LIST& value);
extern bool HandlePacket_LoginGroup(LoginGroup* group, Player& player, Packet& pkt);
extern bool HandlePacket_LobbyGroup(LobbyGroup* group, Player& player, Packet& pkt);
extern bool HandlePacket_GameGroup_Town(GameGroup_Town* group, Player& player, Packet& pkt);
extern Packet& Make_S_GAME_LOGIN(const bool status);
extern Packet& Make_S_GET_CHARACTER_LIST(const vector<Dto_S_GET_CHARACTER_LIST>& charaterList);
extern Packet& Make_S_CREATE_CHARACTER(const bool status);
extern Packet& Make_S_GAME_ENTER(const uint64 characterId, const wstring& nickname, const int32 level, const int32 exp, const float32 y, const float32 x, const int32 hp, const int32 speed, const int32 fieldId);
extern Packet& Make_S_SPAWN_CHARACTER(const uint64 playerId, const wstring& nickname, const int32 level, const float32 y, const float32 x, const int32 hp, const int32 speed, const int32 modelId, const int32 weaponId);
extern Packet& Make_S_DESTROY_CHARACTER(const uint64 playerId);
extern Packet& Make_S_MOVE(const uint64 playerId, const float32 y, const float32 x);
extern Packet& Make_S_MOVE_STOP(const uint64 playerId, const float32 y, const float32 x);