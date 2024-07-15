#pragma once
#include "GameGroupBase.h"

class LobbyGroup : public GameGroupBase
{
protected:
	void OnEnter(uint64 sessionId) override;
	void OnLeave(uint64 sessionId) override;
	void OnRecv(uint64 sessionId, Packet& packet) override;
	void UpdateFrame() override;


private:
	//@@@AutoPackBegin

	friend bool HandlePacket_LobbyGroup(LobbyGroup*, Player&, Packet&);
	void Handle_C_GET_CHARACTER_LIST(Player& player);
	void Handle_C_CREATE_CHARACTER(Player& player, wstring& nickname, int32 modelId, int32 weaponId);
	void Handle_C_GAME_ENTER(Player& player, uint64 characterId, int32 idx);

	//@@@AutoPackEnd
public:
	~LobbyGroup() override;
};

