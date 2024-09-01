#pragma once
#include "../Libs/hiredis/CRedisConn.h"
#include "mysqlx/xdevapi.h"
#include "ObjectPoolTls.h"
#include "ServerBase.h"
#include "Common/Token.h"

namespace login
{
	class Player;

	class LoginServer : public ServerBase
	{
		enum
		{
			NUM_OF_USER_MAP = 128,
			TIMEOUT_MS = 10000,
		};

	public:
		LoginServer(const json& setting);
		~LoginServer() override;

		uint64 GetAuthCount() const { return _authCount; }

	protected:
		void OnStart() override;
		void OnStop() override;
		bool OnConnectionRequest(const NetworkAddress& netInfo) override;
		void OnAccept(const NetworkAddress& netInfo, uint64 sessionId) override;
		void OnDisconnect(uint64 sessionId) override;
		void OnRecv(uint64 sessionId, Packet pkt) override;

		//@@@AutoPackBegin

		friend bool HandlePacket_LoginServer(LoginServer*, Player&, Packet);

		void Handle_C_REQ_LOGIN(Player& player, int64 accountNo, Token& token);
		void Handle_C_GET_SERVER_LIST(Player& player);

		//@@@AutoPackEnd

	private:
		Player* find_player(uint64 sessionId);
		void	delete_player(uint64 sessionId);
		void	insert_player(uint64 sessionId);
		bool	set_token(uint64 accountNo, const Token& token);

		void	func_timeout_thread();


	private:
		//typedef
		using PlayerMapType		= unordered_map<uint64, Player*>;
		using PlayerAllocator	= ObjectPoolTls<Player, false>;
		using RedisSession		= RedisCpp::CRedisConn;

		PlayerMapType	_playerMap[NUM_OF_USER_MAP]; ///< 플레이어 관리하는 MAP
		Lock			_mapLock[NUM_OF_USER_MAP];

		uint64			_authCount;					 ///< 누적 인증 횟수

		vector<uint16>	_gameServerPorts;

		RedisSession*	_redis;
		Lock			_lockRedis;
		PlayerAllocator	_playerAllocator;
	};


}
