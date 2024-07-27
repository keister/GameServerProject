#include "stdafx.h"
#include "GameServer.h"
#include "LobbyGroup.h"

#include "Character.h"
#include "DBJob.h"
#include "DBWriter.h"
#include "Job.h"
#include "PacketHandler.h"
#include "Player.h"


void LobbyGroup::OnRecv(uint64 sessionId, Packet& packet)
{
	Player* player = find_player(sessionId);
	if (player == nullptr)
	{
		return;
	}

	HandlePacket_LobbyGroup(this, *player, packet);
}


void LobbyGroup::Handle_C_GET_CHARACTER_LIST(Player& player)
{
	vector<Dto_S_GET_CHARACTER_LIST> list(player.numCharacters);

	for (int32 i = 0; i < player.numCharacters; i++)
	{
		Character* character = player.characters[i];
		list[i].id = character->Id();
		list[i].level = character->Level();
		list[i].nickname = character->Nickname();
		list[i].modelId = character->ModelId();
		list[i].weaponId = character->WeaponId();
	}

	SendPacket(player.sessionId, Make_S_GET_CHARACTER_LIST(list));
}

void LobbyGroup::Handle_C_CREATE_CHARACTER(Player& player, wstring& nickname, int32 modelId, int32 weaponId)
{
	Character* character = player.AddCharacter(nickname, modelId, weaponId);
	DBWriter::Write(Job::Alloc<DBJob::CreateCharacter>(character->Id(), character->PlayerId(), character->Idx(), character->Nickname(), character->ModelId(), character->WeaponId()));
	SendPacket(player.sessionId, Make_S_CREATE_CHARACTER(true));
}

void LobbyGroup::Handle_C_GAME_ENTER(Player& player, uint64 characterId, int32 idx)
{
	player.curCharacter = player.characters[idx];
	if (player.curCharacter->Id() != characterId)
	{
		return;
	}

	MoveGroup(player.sessionId, player.curCharacter->FieldId());
}


LobbyGroup::~LobbyGroup()
{
}
