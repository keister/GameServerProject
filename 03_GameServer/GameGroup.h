#pragma once
#include "GameGroupBase.h"


namespace game
{
	class FieldManager;
	class Monster;
	class Map;

	/// @brief 실제 게임 필드 로직이 돌아가는 그룹, 하나의 필드당 하나의 그룹을 가진다.
	class GameGroup : public GameGroupBase
	{
	protected:
		void OnRecv(uint64 sessionId, Packet packet) override;

		//@@@AutoPackBegin

		friend bool HandlePacket_GameGroup(GameGroup*, Player&, Packet);
		void Handle_C_MOVE(Player& player, float32 y, float32 x);
		void Handle_C_SKILL(Player& player, uint64 objectId, uint32 skillId);
		void Handle_C_MOVE_FIELD(Player& player, uint8 fieldId);
		void Handle_C_CHARACTER_RESPAWN(Player& player);

		//@@@AutoPackEnd

	public:
		GameGroup();
		~GameGroup() override;
		void Init() override;

	protected:
		void OnPlayerEnter(Player& player) override;
		void OnPlayerLeave(Player& player) override;

	private:
		FieldManager* _field;
	};
}


