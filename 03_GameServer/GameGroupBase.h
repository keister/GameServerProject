#pragma once
#include "GameObject.h"
#include "GlobalPool.h"
#include "GroupBase.h"
#include "TimerEvent.h"
struct TimerEvent;
class GameObject;
class GameServer;
class Player;


class GameGroupBase : public GroupBase
{
public:
	GameGroupBase() {};

	void SetServer(GameServer* server);
	uint64 GetPlayerCount() { return _players.size(); }
protected:
	template <typename T, typename ...Args>
	T* CreateObject(Args... args)
	{
		uint64 id = InterlockedIncrement64((LONGLONG*)&_objectIdGenerator);

		GameObject* ret;
		if constexpr (is_base_of_v<enable_object_pooling, T>)
		{
			ret = GlobalPool<T>::Alloc(args...);
			ret->_doPooling = true;
			ret->_freeFunc = GlobalPool<T>::Free;
			
		}
		else
		{
			ret = new T;
			ret->_doPooling = false;
		}

		ret->_objectId = id;
		ret->_owner = this;
		ret->position = { 0, 0 };
		ret->yaw = 0.f;

		_gameObjects.insert(ret);

		return (T*)ret;
	}

	void DestroyObject(GameObject* object)
	{
		_removeObjects.push_back(object);
	}

	void ReleaseObject(GameObject* object)
	{
		
		if (object->_doPooling == true)
		{
			object->_freeFunc(object);
		}
		else
		{
			delete object;
		}
	}

	Player* find_player(uint64 sessionId);
	void insert_player(Player* player);
	void delete_player(uint64 sessionId);
	virtual void OnPlayerEnter(Player& player) {};
	virtual void OnPlayerLeave(Player& player) {};
	void Invoke(function<void()>&& func, DWORD afterTick);
	void Invoke(function<void()>&& func, float32 afterTime);

public:
	~GameGroupBase() override {};

protected:
	void OnEnter(uint64 sessionId) final;
	void OnLeave(uint64 sessionId) final;
	void UpdateFrame() final;
	GameServer* _server;
private:
	void process_timer_event();

	priority_queue<TimerEvent> _timerEventQueue;
	list<GameObject*> _removeObjects;
	uint64 _objectIdGenerator = 0;
	unordered_set<GameObject*> _gameObjects;
	unordered_map<uint64, Player*> _players;

};

