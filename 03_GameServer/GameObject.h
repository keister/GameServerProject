#pragma once
#include "GlobalPool.h"
#include "InheritanceReflection.h"
class GameHost;
class Packet;
class Sector;
class GameGroupBase;



class enable_object_pooling
{
	
};

class GameObject
{
	friend GameGroupBase;
	friend GameHost;

public:
	using RootType = void;
	using SuperType = RootType;
	using ThisType = GameObject;


	static TypeInfo* StaticInfo()
	{
		static TypeInfo info = TypeInfo(TypeInfoInitializer<ThisType>("GameObject"));

		return &info;
	}

	virtual const TypeInfo* GetTypeInfo() { return  _typeInfo; }

	inline static TypeInfo* _typeInfo = StaticInfo();
public:
	GameObject(){};
	virtual ~GameObject(){};
	void SendPacket(uint64 sessionId, Packet pkt);
	void SendPacket(int32 sectorRange, Packet pkt);
	void SendPacket(int32 sectorRange, Packet pkt, uint64 except);
	void SendPacket(int32 sectorRange, Packet pkt, list<uint64>& exceptSession);

	template <typename T>
	bool ExecuteForEachHost(int32 sectorRange, std::function<void(T*)>&& func);

	template <typename T>
	bool ExecuteForEachObject(int32 sectorRange, std::function<void(T*)>&& func);


protected:
	float32 DeltaTime();

	template <typename T, typename ...Args>
	T* CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args);



	void DestroyObject(GameObject* object);

	void Invoke(function<void()>&& func, DWORD afterTick);
	void Invoke(function<void()>&& func, float32 afterTime);

	void MovePosition(float32 x, float32 y);

	void MoveTowards(const Eigen::Vector2<float32>& target, float32 maxDistance);
	void LookAt(const Eigen::Vector2<float32>& target);

	virtual void OnUpdate() {}
	// virtual void OnSectorLeave(int32 sectorRange) {}
	// virtual void OnSectorEnter(int32 sectorRange) {}
	virtual void OnSpawnRequest(const list<GameHost*>& sessionList) {}
	virtual void OnDestroyRequest(const list<GameHost*>& sessionList) {}


public:
	Eigen::Vector2<float32> position;
	float32 yaw;

protected:
	GameHost* _host = nullptr;

private:
	uint64 _objectId;
	GameGroupBase* _owner;
	Sector* _sector;
	bool _doPooling;
	bool _isDestroy = false;

	std::function<void(void*)> _freeFunc;
};

class FixedObject
{
	friend GameGroupBase;

public:
	FixedObject() {}
	virtual ~FixedObject() {}

	void SendPacket(uint64 sessionId, Packet pkt);
	float32 DeltaTime();
protected:
	template <typename T, typename ...Args>
	T* CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args);
	void DestroyObject(GameObject* object);
	void Invoke(function<void()>&& func, DWORD afterTick);
	void Invoke(function<void()>&& func, float32 afterTime);

	virtual void OnUpdate() {}
private:
	uint64 _objectId;
	GameGroupBase* _owner;

	bool _doPooling;
	bool _isDestroy = false;
	std::function<void(void*)> _freeFunc;
};


#include "GameGroupBase.h"

template <typename T>
bool GameObject::ExecuteForEachHost(int32 sectorRange, std::function<void(T*)>&& func)
{
	return _owner->ExecuteForEachHost<T>(_sector, sectorRange, std::move(func));

}

template <typename T>
bool GameObject::ExecuteForEachObject(int32 sectorRange, std::function<void(T*)>&& func)
{
	return _owner->ExecuteForEachObject<T>(_sector, sectorRange, std::move(func));
}

template <typename T, typename ... Args>
T* GameObject::CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args)
{
	return _owner->CreateObject<T>(pos, args...);
}

template <typename T, typename ... Args>
T* FixedObject::CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args)
{
	return _owner->CreateObject<T>(pos, args...);
}
