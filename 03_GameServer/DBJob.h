#pragma once
#include "Job.h"

namespace game
{
	namespace DBJob
	{
		class CreateCharacter : public Job
		{
		public:
			CreateCharacter(uint64 id, uint64 playerId, int32 idx, const wstring& nickname, int32 modelId, int32 weaponId)
				: _id(id),
				_playerId(playerId),
				_idx(idx),
				_nickname(nickname),
				_modelId(modelId),
				_weaponId(weaponId)
			{
			}

			void Execute() override;

		private:
			uint64	_id;
			uint64	_playerId;
			int32	_idx;
			wstring _nickname;
			int32	_modelId;
			int32	_weaponId;
		};
	}

}
