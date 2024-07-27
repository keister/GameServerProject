#pragma once
#include "GlobalPool.h"
class Packet;
class Sector;
class GameGroupBase;


class enable_object_pooling
{
	
};

class GameObject
{
	friend GameGroupBase;
public:
	GameObject(){};
	virtual ~GameObject(){};
	

protected:
	float32 DeltaTime();

	void SendPacket(uint64 sessionId, Packet& pkt);
	void SendPacket(int32 sectorRange, Packet& pkt);
	void SendPacket(int32 sectorRange, Packet& pkt, list<uint64> exceptSession);
	void Invoke(function<void()>&& func, DWORD afterTick);
	void Invoke(function<void()>&& func, float32 afterTime);
	virtual void OnUpdate() {};

protected:
	Eigen::Vector2<float32> position;
	float32 yaw;


private:
	uint64 _objectId;
	GameGroupBase* _owner;
	Sector* _sector;
	bool _doPooling;

	std::function<void(void*)> _freeFunc;
};

