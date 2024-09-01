#include "stdafx.h"
#include "GameServer.h"
#include "LobbyGroup.h"

#include "Character.h"
#include "DBJob.h"
#include "DBWriter.h"
#include "Job.h"
#include "PacketHandler.h"
#include "Player.h"
#include "SqlSession.h"


void game::LobbyGroup::OnRecv(uint64 sessionId, Packet packet)
{
	Player* player = FindPlayer(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_LobbyGroup(this, *player, packet);
}


void game::LobbyGroup::Handle_C_GET_CHARACTER_LIST(Player& player)
{
	vector<Dto_S_GET_CHARACTER_LIST> list(player.numCharacters);

	for (int32 i = 0; i < list.size(); i++)
	{
		CharacterInfo& character = player.characterInfos[i];
		list[i].id = character.id;
		list[i].level = character.level;
		list[i].nickname = character.nickname;
		list[i].modelId = character.modelId;
		list[i].weaponId = character.weaponId;
		list[i].fieldId = character.fieldId;
	}

	SendPacket(player.SessionId(), Make_S_GET_CHARACTER_LIST(list));
}

void game::LobbyGroup::Handle_C_CREATE_CHARACTER(Player& player, wstring& nickname, int32 modelId, int32 weaponId)
{
	CharacterInfo& info = player.AddCharacter(nickname, modelId, weaponId);

	uint64 playerId = player.id;
	uint64 sessionId = player.SessionId();

	DBWriter::Write(
		Job::Alloc([this, info, playerId, sessionId]
			{
				mysqlx::Schema sch = GetSqlSession().getSchema("game");

				try
				{
					sch.getTable("character")
						.insert("id", "player_id", "idx", "nickname", "model_id", "weapon_id")
						.values(info.id, playerId, info.idx, info.nickname, info.modelId, info.weaponId)
						.execute();
				}
				catch (const mysqlx::Error& e)
				{
					cout << e << endl;
				}

				SendPacket(sessionId, Make_S_CREATE_CHARACTER(true));
			}
		)
	);

}

void game::LobbyGroup::Handle_C_GAME_ENTER(Player& player, uint64 characterId, int32 idx)
{
	player.curIndex = idx;
	player.spawnInfo = player.characterInfos[player.curIndex].pos;
	MoveGroup(player.SessionId(), player.characterInfos[player.curIndex].fieldId);

	SendPacket(player.SessionId(), Make_S_GAME_ENTER(
		player.id,
		player.characterInfos[player.curIndex].id
	));
}


game::LobbyGroup::~LobbyGroup()
{
}
