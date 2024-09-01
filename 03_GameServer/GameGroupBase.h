#pragma once
#include "GlobalPool.h"
#include "GroupBase.h"



#include "TimerEvent.h"

namespace game
{
	class Route;
	struct RouteNode;
	class MapData;
	class FixedObject;
	class Sector;
	struct TimerEvent;
	class GameServer;
	class Player;
	class GameObject;
	class FixedObject;
	class Map;

	class GameGroupBase : public netlib::GroupBase
	{
	public:
		GameGroupBase() : _server(nullptr), _map(nullptr) {};

		void SetServer(GameServer* server);
		uint64 GetPlayerCount() { return _players.size(); }

		uint64 GetObjectCount() { return _gameObjects.size(); }

		bool CreateMap(const char* fileName, int32 sectorWidth, int32 sectorHeight);

		template <typename T, typename ...Args>
		T* CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args);

		template <typename T, typename ...Args>
		T* CreateFixedObject(Args&&... args);

		void DestroyObject(GameObject* object);

		void DestroyFixedObject(FixedObject* object);

		void Invoke(BaseObject* object, function<void()>&& func, DWORD afterTick);
		void Invoke(BaseObject* object, function<void()>&& func, float32 afterTime);

		Sector* FindSectorByPostion(float32 x, float32 y);

		void SendPacket(uint64 sessionId, Packet pkt);
		void SendPacket(Sector* sector, int32 sectorRange, Packet pkt);
		void SendPacket(Sector* sector, int32 sectorRange, Packet pkt, uint64 except);
		void SendPacket(Sector* sector, int32 sectorRange, Packet pkt, list<uint64>& exceptSession);

		template <typename T>
		bool ExecuteForEachHost(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func);

		template <typename T>
		bool ExecuteForEachObject(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func);
		void PathFindingCompletionRoutine(GameObject* gameObject, uint64 objectId, uint64 execCount, Route& route);
		Map* GetMap() const { return _map; }

	protected:
		void ReleaseObject(GameObject* object);
		void ReleaseFixedObject(FixedObject* object);
		virtual void OnPlayerEnter(Player& player) {};
		virtual void OnPlayerLeave(Player& player) {};

	public:
		~GameGroupBase() override {};

	protected:
		void OnEnter(uint64 sessionId) final;
		void OnLeave(uint64 sessionId) final;
		Player* FindPlayer(uint64 sessionId);
		void	InsertPlayer(Player* player);
		void	DeletePlayer(uint64 sessionId);
		void UpdateFrame() final;
		GameServer* _server;
	private:
		void	process_timer_event();


	private:
		using TimerEventQueue		= priority_queue<TimerEvent>;
		using GameObjectSetType		= unordered_set<GameObject*>;
		using FixedObjectSetType	= unordered_set<FixedObject*>;
		using PlayerMapType			= unordered_map<uint64, Player*>;

		priority_queue<TimerEvent>	_timerEventQueue;
		list<GameObject*>			_removeObjects;
		list<FixedObject*>			_removefixedObjects;

		GameObjectSetType			_gameObjects;
		FixedObjectSetType			_fixedObjects;
		PlayerMapType				_players;
		Map*						_map;

		inline static uint64 _objectIdGenerator = 0;
	};
}

#include "GameObject.h"
#include "Map.h"


template<typename T>
inline bool game::GameGroupBase::ExecuteForEachHost(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func)
{
	if (_map == nullptr)
	{
		return false;
	}

	_map->ExecuteForEachHost<T>(sector, sectorRange, std::move(func));

	return true;
}

template <typename T>
bool game::GameGroupBase::ExecuteForEachObject(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func)
{
	if (_map == nullptr)
	{
		return false;
	}

	_map->ExecuteForEachObject<T>(sector, sectorRange, std::move(func));

	return true;
}


template <typename T, typename ... Args>
T* game::GameGroupBase::CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args)
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
		ret = (GameObject*)new T(args...);
		ret->_doPooling = false;
	}

	ret->_objectId = id;
	ret->_owner = this;
	ret->position = pos;
	ret->_sector = FindSectorByPostion(pos.x(), pos.y());
	ret->_sector->InsertObject(ret);
	ret->yaw = 0.f;
	ret->_host = nullptr;
	list<GameHost*> other;
	ret->ExecuteForEachHost<GameHost>(AROUND,
		[&other](GameHost* ho)
		{
			other.push_back(ho);
		});

	ret->OnSpawnRequest(other);

	_gameObjects.insert(ret);

	return (T*)ret;
}

template <typename T, typename ... Args>
T* game::GameGroupBase::CreateFixedObject(Args&&... args)
{
	uint64 id = InterlockedIncrement64((LONGLONG*)&_objectIdGenerator);

	FixedObject* ret;

	if constexpr (is_base_of_v<enable_object_pooling, T>)
	{
		ret = GlobalPool<T>::Alloc(args...);
		ret->_doPooling = true;
		ret->_freeFunc = GlobalPool<T>::Free;

	}
	else
	{
		ret = (FixedObject*)new T(args...);

		ret->_doPooling = false;
	}

	ret->_objectId = id;
	ret->_owner = this;

	_fixedObjects.insert(ret);

	return (T*)ret;

}

