#pragma once

enum class PacketType : uint16
{
	LOGIN_SERVER_PACKET = 100,
	 
	//----------------------------------------------------------------
	/// Login 요청\n
	///---------------------------------------------------------------
	/// - int64 accountNo
	/// - Token token
	///
	//----------------------------------------------------------------
	C_REQ_LOGIN, 
	 
	//----------------------------------------------------------------
	/// Login 응답\n
	///---------------------------------------------------------------
	/// - int64 accountNo
	/// - bool status
	/// - Token token
	///
	//----------------------------------------------------------------
	S_RES_LOGIN, 
	 
	//----------------------------------------------------------------
	/// 서버 목록 요청\n
	///---------------------------------------------------------------
	///
	//----------------------------------------------------------------
	C_GET_SERVER_LIST, 
	 
	//----------------------------------------------------------------
	/// 서버 목록 응답\n
	///---------------------------------------------------------------
	/// - vector<wstring> gameServerIps
	/// - vector<uint16> gameServerPorts
	///
	//----------------------------------------------------------------
	S_GET_SERVER_LIST, 
	 
	 
	CHAT_SERVER_PACKET = 200,
	 
	//----------------------------------------------------------------
	/// 채팅서버 로그인 요청\n
	///---------------------------------------------------------------
	/// - uint64 accountId
	/// - Token token
	///
	//----------------------------------------------------------------
	C_CHAT_LOGIN, 
	 
	//----------------------------------------------------------------
	/// 채팅서버 로그인 응답\n
	///---------------------------------------------------------------
	/// - bool status
	///
	//----------------------------------------------------------------
	S_CHAT_LOGIN, 
	 
	//----------------------------------------------------------------
	/// 채팅 입장\n
	///---------------------------------------------------------------
	/// - uint64 characterId
	/// - int32 fieldId
	///
	//----------------------------------------------------------------
	C_CHAT_ENTER, 
	 
	//----------------------------------------------------------------
	/// 채팅 입장 응답\n
	///---------------------------------------------------------------
	/// - bool status
	///
	//----------------------------------------------------------------
	S_CHAT_ENTER, 
	 
	//----------------------------------------------------------------
	/// 채팅 퇴장\n
	///---------------------------------------------------------------
	///
	//----------------------------------------------------------------
	C_CHAT_LEAVE, 
	 
	//----------------------------------------------------------------
	/// 필드이동\n
	///---------------------------------------------------------------
	/// - int32 fieldId
	///
	//----------------------------------------------------------------
	C_MOVE_FIELD, 
	 
	//----------------------------------------------------------------
	/// 같은 필드내의 플레이어에게 채팅\n
	///---------------------------------------------------------------
	/// - uint8 chatType
	/// - wstring message
	///
	//----------------------------------------------------------------
	C_CHAT, 
	 
	//----------------------------------------------------------------
	/// 채팅 응답\n
	///---------------------------------------------------------------
	/// - uint8 chatType
	/// - uint64 playerId
	/// - uint64 characterId
	/// - wstring nickname
	/// - wstring message
	///
	//----------------------------------------------------------------
	S_CHAT, 
	 
	//----------------------------------------------------------------
	/// 귓속말\n
	///---------------------------------------------------------------
	/// - wstring toNickname
	/// - wstring message
	///
	//----------------------------------------------------------------
	C_WHISPER, 
	 
	//----------------------------------------------------------------
	/// 귓속말 응답\n
	///---------------------------------------------------------------
	/// - uint64 playerId
	/// - uint64 characterId
	/// - wstring nickname
	/// - wstring message
	///
	//----------------------------------------------------------------
	S_WHISPER, 
	 
	//----------------------------------------------------------------
	/// 채팅 실패 에러메시지\n
	///---------------------------------------------------------------
	/// - int32 code
	///
	//----------------------------------------------------------------
	S_CHAT_FAIL, 
	 
	 
	GAME_SERVER_PACKET = 300,
	 
	//----------------------------------------------------------------
	/// 게임서버 로그인 요청\n
	///---------------------------------------------------------------
	/// - uint64 accountId
	/// - Token token
	///
	//----------------------------------------------------------------
	C_GAME_LOGIN, 
	 
	//----------------------------------------------------------------
	/// 게임서버 로그인 응답\n
	///---------------------------------------------------------------
	/// - bool status
	///
	//----------------------------------------------------------------
	S_GAME_LOGIN, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 목록 요청\n
	///---------------------------------------------------------------
	///
	//----------------------------------------------------------------
	C_GET_CHARACTER_LIST, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 목록 응답\n
	///---------------------------------------------------------------
	/// - vector<Dto_S_GET_CHARACTER_LIST> charaterList
	///	@ref Dto_S_GET_CHARACTER_LIST
	///		- uint64 id
	///		- wstring nickname
	///		- int32 level
	///		- int32 modelId
	///		- int32 weaponId
	///		- int32 fieldId
	///
	//----------------------------------------------------------------
	S_GET_CHARACTER_LIST, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 생성 요청\n
	///---------------------------------------------------------------
	/// - wstring nickname
	/// - int32 modelId
	/// - int32 weaponId
	///
	//----------------------------------------------------------------
	C_CREATE_CHARACTER, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 생성 응답\n
	///---------------------------------------------------------------
	/// - bool status
	///
	//----------------------------------------------------------------
	S_CREATE_CHARACTER, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 선택후 게임입장\n
	///---------------------------------------------------------------
	/// - uint64 characterId
	/// - int32 idx
	///
	//----------------------------------------------------------------
	C_GAME_ENTER, 
	 
	//----------------------------------------------------------------
	/// 게임 입장 응답\n
	///---------------------------------------------------------------
	/// - uint64 playerId
	/// - uint64 characterId
	///
	//----------------------------------------------------------------
	S_GAME_ENTER, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 생성\n
	///---------------------------------------------------------------
	/// - uint64 playerId
	/// - wstring nickname
	/// - int32 level
	/// - float32 y
	/// - float32 x
	/// - int32 maxHp
	/// - int32 hp
	/// - int32 maxMp
	/// - int32 mp
	/// - int32 speed
	/// - int32 modelId
	/// - int32 weaponId
	/// - float32 rotation
	///
	//----------------------------------------------------------------
	S_SPAWN_CHARACTER, 
	 
	//----------------------------------------------------------------
	/// 캐릭터 삭제\n
	///---------------------------------------------------------------
	/// - uint64 playerId
	///
	//----------------------------------------------------------------
	S_DESTROY_CHARACTER, 
	 
	//----------------------------------------------------------------
	/// 플레이어 이동\n
	///---------------------------------------------------------------
	/// - float32 y
	/// - float32 x
	///
	//----------------------------------------------------------------
	C_MOVE, 
	 
	//----------------------------------------------------------------
	/// 다른 플레이어에게 위치 전파\n
	///---------------------------------------------------------------
	/// - uint64 playerId
	/// - float32 y
	/// - float32 x
	///
	//----------------------------------------------------------------
	S_MOVE, 
	 
	//----------------------------------------------------------------
	/// 공격 보고\n
	///---------------------------------------------------------------
	/// - uint64 objectId
	/// - uint32 skillId
	///
	//----------------------------------------------------------------
	C_SKILL, 
	 
	//----------------------------------------------------------------
	/// 공격 전파\n
	///---------------------------------------------------------------
	/// - uint64 playerId
	/// - uint32 skillId
	/// - float32 targetY
	/// - float32 targetX
	/// - float32 posY
	/// - float32 posX
	///
	//----------------------------------------------------------------
	S_SKILL, 
	 
	//----------------------------------------------------------------
	/// 몬스터 생성\n
	///---------------------------------------------------------------
	/// - uint64 id
	/// - uint64 objectId
	/// - int32 hp
	/// - int32 speed
	/// - float32 y
	/// - float32 x
	///
	//----------------------------------------------------------------
	S_SPAWN_MONSTER, 
	 
	//----------------------------------------------------------------
	/// 오브젝트 파괴\n
	///---------------------------------------------------------------
	/// - uint8 type
	/// - uint64 id
	///
	//----------------------------------------------------------------
	S_DESTROY_OBJECT, 
	 
	//----------------------------------------------------------------
	/// 오브젝트 이동\n
	///---------------------------------------------------------------
	/// - uint8 type
	/// - uint64 id
	/// - float32 y
	/// - float32 x
	///
	//----------------------------------------------------------------
	S_MOVE_OBJECT, 
	 
	//----------------------------------------------------------------
	/// 오브젝트 피해\n
	///---------------------------------------------------------------
	/// - uint8 type
	/// - uint64 id
	/// - int32 hp
	///
	//----------------------------------------------------------------
	S_DAMAGE, 
	 
	//----------------------------------------------------------------
	/// 경험치 획득\n
	///---------------------------------------------------------------
	/// - int32 curExp
	///
	//----------------------------------------------------------------
	S_EXP, 
	 
	 
};



inline RawPacket& operator<< (RawPacket& pkt, PacketType type)
{
	pkt << static_cast<uint16>(type);

	return pkt;
}

inline RawPacket& operator>> (RawPacket& pkt, PacketType& type)
{
	pkt >> *reinterpret_cast<uint16*>(&type);

	return pkt;
}