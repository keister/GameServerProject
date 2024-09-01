#pragma once
#include "GameGroupBase.h"

struct Token;


namespace game
{
	/// @brief ���� �� ���� ���� DB�ε��� ����ϴ� �׷�
	class LoginGroup : public GameGroupBase
	{
	protected:
		void OnRecv(uint64 sessionId, Packet packet) override;


	private:
		//@@@AutoPackBegin

		friend bool HandlePacket_LoginGroup(LoginGroup*, Player&, Packet);
		void Handle_C_GAME_LOGIN(Player& player, uint64 accountId, Token& token);

		//@@@AutoPackEnd


	public:
		~LoginGroup() override;
	};
}


