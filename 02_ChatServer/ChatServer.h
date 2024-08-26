#pragma once
#include "DBReadThreadPool.h"
#include "ServerBase.h"
#include "Player.h"

#include "Libs/hiredis/CRedisConn.h"
class Job;
struct Token;
class Player;
using RedisSession = RedisCpp::CRedisConn;

class ChatServer : public ServerBase
{
	enum
	{
		NUM_OF_FILEDS = 10,
	};

	enum
	{
		NO_WHISPER_TARGET
	};

	enum ChatType
	{
		FIELD,

	};

public:
	ChatServer(const wstring& name);
	~ChatServer() override;
	bool GetToken(uint64 accountId, Token& token);
protected:
	void OnStart() override;
	void OnStop() override;
	bool OnConnectionRequest(const NetworkAddress& netInfo) override;
	void OnAccept(const NetworkAddress& netInfo, uint64 sessionId) override;
	void OnDisconnect(uint64 sessionId) override;

	Player* FindPlayer(uint64 sessionId);
	void DeletePlayer(Player& player);
	void InsertPlayer(Player& player);

public:



protected:
	void OnRecv(uint64 sessionId, Packet pkt) override;

	//@@@AutoPackBegin
	friend bool HandlePacket_ChatServer(ChatServer*, Player&, Packet);
	void Handle_C_CHAT_LOGIN(Player& player, uint64 accountId, Token& token);
	void Handle_C_CHAT_ENTER(Player& player, uint64 characterId);
	void Handle_C_CHAT_LEAVE(Player& player);
	void Handle_C_CHAT_MOVE_FIELD(Player& player, int32 fieldId);
	void Handle_C_CHAT(Player& player, uint8 chatType, wstring& message);
	void Handle_C_WHISPER(Player& player, wstring& toNickname, wstring& message);

	//@@@AutoPackEnd

private:


private:
	ObjectPoolTls<Player, false> _playerPool;
	DBReadThreadPool _dbRead;

	unordered_map<uint64, Player*> _players;
	unordered_map<wstring, Player*> _nicknameToPlayer;
	Lock _playerMaplock;
	Lock _nicknameToPlayerLock;

	RedisSession* _redis;
	Lock _redisLock;

	unordered_set<Player*> _playersOnField[NUM_OF_FILEDS];
	Lock _fieldLock[NUM_OF_FILEDS];
};

