#pragma once
#include "GameGroupBase.h"

struct Token;

class LoginGroup : public GameGroupBase
{
protected:
	void OnEnter(uint64 sessionId) override;
	void OnLeave(uint64 sessionId) override;
	void OnRecv(uint64 sessionId, Packet& packet) override;
	void UpdateFrame() override;


private:
	//@@@AutoPackBegin

	friend bool HandlePacket_LoginGroup(LoginGroup*, Player&, Packet&);
	void Handle_C_GAME_LOGIN(Player& player, uint64 accountId, Token& token);

	//@@@AutoPackEnd


public:
	~LoginGroup() override;
};

