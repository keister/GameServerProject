#pragma once
#include "DBReadThreadPool.h"
#include "ServerBase.h"
#include "Player.h"

#include "Libs/hiredis/CRedisConn.h"


struct Token;

namespace netlib
{
	class Job;
}

namespace chat
{
	

	class Player;
	using RedisSession = RedisCpp::CRedisConn;

	class ChatServer : public netlib::ServerBase
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
		ChatServer(const json& setting);
		~ChatServer() override;

	protected:
		void OnStart() override;
		void OnStop() override;
		bool OnConnectionRequest(const NetworkAddress& netInfo) override;
		void OnAccept(const NetworkAddress& netInfo, uint64 sessionId) override;
		void OnDisconnect(uint64 sessionId) override;

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
		Player* find_player(uint64 sessionId);
		void	delete_player(Player& player);
		void	insert_player(Player& player);
		bool	get_token(uint64 accountId, Token& token);

	private:
		//typedef
		using PlayerAllocator			= ObjectPoolTls<Player, false>;
		using PlayerSetType				= unordered_set<Player*>;
		using PlayerMapType				= unordered_map<uint64, Player*>;
		using NicknameToPlayerMapType	= unordered_map<wstring, Player*>;

		PlayerAllocator			_playerPool;
		DBReadThreadPool		_dbRead;

		PlayerMapType			_players;
		Lock					_playerMaplock;
		NicknameToPlayerMapType	_nicknameToPlayer;
		Lock					_nicknameToPlayerLock;
		PlayerSetType			_playersOnField[NUM_OF_FILEDS];
		Lock					_fieldLock[NUM_OF_FILEDS];

		RedisSession*			_redis;
		Lock					_redisLock;


	};
}

