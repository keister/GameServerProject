#include "stdafx.h"
#include "GameServer.h"
#include "LobbyGroup.h"

#include "Character.h"
#include "DBJob.h"
#include "DBWriter.h"
#include "Job.h"
#include "PacketHandler.h"
#include "Player.h"


void LobbyGroup::OnRecv(uint64 sessionId, Packet packet)
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

	for (int32 i = 0; i < list.size(); i++)
	{
		CharacterInfo& character = player.characterInfos[i];
		list[i].id = character.id;
		list[i].level = character.level;
		list[i].nickname = character.nickname;
		list[i].modelId = character.modelId;
		list[i].weaponId = character.weaponId;
	}

	SendPacket(player.SessionId(), Make_S_GET_CHARACTER_LIST(list));
}

void LobbyGroup::Handle_C_CREATE_CHARACTER(Player& player, wstring& nickname, int32 modelId, int32 weaponId)
{
	CharacterInfo& info = player.AddCharacter(nickname, modelId, weaponId);
	DBWriter::Write(Job::Alloc<DBJob::CreateCharacter>(
		info.id, 
		player.id, 
		info.idx, 
		info.nickname, 
		info.modelId, 
		info.weaponId
	));

	SendPacket(player.SessionId(), Make_S_CREATE_CHARACTER(true));
}

void LobbyGroup::Handle_C_GAME_ENTER(Player& player, uint64 characterId, int32 idx)
{
	player.curIndex = idx;

	MoveGroup(player.SessionId(), player.characterInfos[player.curIndex].fieldId);
}


LobbyGroup::~LobbyGroup()
{
}
