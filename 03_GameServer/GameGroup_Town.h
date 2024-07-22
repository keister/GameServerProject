#pragma once
#include "GameGroupBase.h"


class Map;

class GameGroup_Town : public GameGroupBase
{
protected:
	void OnEnter(uint64 sessionId) override;
	void OnLeave(uint64 sessionId) override;
	void OnRecv(uint64 sessionId, Packet& packet) override;
	void UpdateFrame() override;

	//@@@AutoPackBegin

	friend bool HandlePacket_GameGroup_Town(GameGroup_Town*, Player&, Packet&);
	void Handle_C_MOVE(Player& player, float32 y, float32 x);
	void Handle_C_ATTACK(Player& player, int32 combo);

	//@@@AutoPackEnd

public:
	GameGroup_Town();
	~GameGroup_Town() override;


private:
	Map* _map;
};

