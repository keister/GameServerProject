#pragma once


namespace game
{
	class GameObject;
	class GameServer;

	class GameHost
	{
		friend GameServer;

	public:
		uint64 SessionId() const { return _sessionId; }

		uint64 AttachObject(GameObject* object);
		void   DetachObject();

		GameObject* GetGameObject()
		{
			if (_attachObject == nullptr)
			{
				return nullptr;
			}

			return _attachObject;
		}

		virtual ~GameHost();

	private:
		uint64 _sessionId;
		GameObject* _attachObject;
	};
}


