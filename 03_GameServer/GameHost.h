#pragma once
class GameObject;
class GameServer;

class GameHost
{
	friend GameServer;

public:
	uint64 SessionId() { return _sessionId; }
	uint64 AttachObject(GameObject* object);

	GameObject* GetGameObject()
	{
		if (_attachObject == nullptr)
		{
			return nullptr;
		}

		return _attachObject;
	}

	void OnEnter();
	void OnLeave();

	virtual ~GameHost();

private:
	uint64 _sessionId;
	GameObject* _attachObject;
};


