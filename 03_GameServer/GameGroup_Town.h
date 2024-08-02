#pragma once
#include "GameGroupBase.h"


class FieldManager;
class Monster;
class Map;

class GameGroup_Town : public GameGroupBase
{
protected:
	// void OnEnter(uint64 sessionId) override;
	// void OnLeave(uint64 sessionId) override;
	void OnRecv(uint64 sessionId, Packet packet) override;
	//void UpdateFrame() override;

	//@@@AutoPackBegin

	friend bool HandlePacket_GameGroup_Town(GameGroup_Town*, Player&, Packet);
	void Handle_C_MOVE(Player& player, float32 y, float32 x);
	void Handle_C_SKILL(Player& player, uint64 objectId, uint32 skillId);

	//@@@AutoPackEnd

public:
	GameGroup_Town();
	~GameGroup_Town() override;

protected:
	void OnPlayerEnter(Player& player) override;
	void OnPlayerLeave(Player& player) override;

private:
	FieldManager* _field;
};

