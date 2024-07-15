#include "stdafx.h"
#include "LoginGroup.h"

#include "Character.h"
#include "GameServer.h"
#include "GroupDefine.h"
#include "Player.h"
#include "Packet.h"
#include "PacketHandler.h"
#include "Common/Token.h"
#include "SqlSession.h"

void LoginGroup::OnEnter(uint64 sessionId)
{
	Player* player = _server->FindPlayer(sessionId);
	insert_player(player);
}

void LoginGroup::OnLeave(uint64 sessionId)
{
	delete_player(sessionId);
}

void LoginGroup::OnRecv(uint64 sessionId, Packet& packet)
{
	Player* player = find_player(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_LoginGroup(this, *player, packet);
}

void LoginGroup::UpdateFrame()
{
}

void LoginGroup::Handle_C_GAME_LOGIN(Player& player, uint64 accountId, Token& token)
{
	Token tk;
	_server->GetToken(accountId, tk);

	if (tk != token)
	{
		SendPacket(player.sessionId, Make_S_GAME_LOGIN(false));
		return;
	}

	SqlSession& sql = GetSqlSession();

	mysqlx::Schema sch = sql.getSchema("game");

	mysqlx::RowResult result = sch.getTable("player")
		.select("id")
		.where("account_id = :ID")
		.bind("ID", accountId)
		.execute();

	if (result.count() == 0)
	{
		auto insert = sch.getTable("player")
			.insert("account_id")
			.values(accountId)
			.execute();
		player.id = insert.getAutoIncrementValue();
		player.accountId = accountId;
	}
	else
	{
		mysqlx::Row row = result.fetchOne();

		player.id = row[0].get<uint64>();
		player.accountId = accountId;
	}



	result = sch.getTable("character")
		.select("*")
		.where("player_id = :ID")
		.bind("ID", player.id)
		.execute();

	list<mysqlx::Row> rows = result.fetchAll();

	for (mysqlx::Row r : rows)
	{
		player.AddCharacter(r);
	}
	MoveGroup(player.sessionId, (uint8)Groups::LOBBY);
	SendPacket(player.sessionId, Make_S_GAME_LOGIN(true));
}


LoginGroup::~LoginGroup()
{
}
