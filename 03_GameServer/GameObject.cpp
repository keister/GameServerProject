#include "stdafx.h"
#include "GameObject.h"

#include <numbers>

#include "GameGroupBase.h"
#include "Player.h"

void GameObject::DestroyObject(GameObject* object)
{
	_owner->DestroyObject(object);
}

void GameObject::SendPacket(uint64 sessionId, Packet pkt)
{
	_owner->SendPacket(sessionId, pkt);
}

void GameObject::SendPacket(int32 sectorRange, Packet pkt)
{
	_owner->SendPacket(_sector, sectorRange, pkt);
}

void GameObject::SendPacket(int32 sectorRange, Packet pkt, uint64 except)
{
	_owner->SendPacket(_sector, sectorRange, pkt, except);
}

void GameObject::SendPacket(int32 sectorRange, Packet pkt, list<uint64>& exceptSession)
{
	_owner->SendPacket(_sector, sectorRange, pkt, exceptSession);
}

float32 GameObject::DeltaTime()
{
	return _owner->DeltaTime();
}

void GameObject::Invoke(function<void()>&& func, DWORD afterTick)
{
	_owner->Invoke(std::move(func), afterTick);
}

void GameObject::Invoke(function<void()>&& func, float32 afterTime)
{
	_owner->Invoke(std::move(func), afterTime);
}

void GameObject::MovePosition(float32 x, float32 y)
{
	Sector* newSector = _owner->FindSectorByPostion(x, y);
	if (newSector == _sector)
	{
		position = { x, y };
		return;
	}

	int32 spawnSectorRange = 0;
	int32 destroySectorRange = 0;

	int32 dy = newSector->pos.y - _sector->pos.y;
	int32 dx = newSector->pos.x - _sector->pos.x;

	if (dx == 1)
	{
		spawnSectorRange |= COL_RIGHT;
		destroySectorRange |= COL_LEFT;
	}
	else if (dx == -1)
	{
		spawnSectorRange |= COL_LEFT;
		destroySectorRange |= COL_RIGHT;
	}

	if (dy == 1)
	{
		spawnSectorRange |= ROW_UP;
		destroySectorRange |= ROW_DOWN;
	}
	else if (dy == -1)
	{
		spawnSectorRange |= ROW_DOWN;
		destroySectorRange |= ROW_UP;
	}

	list<GameHost*> destroyList;
	ExecuteForEachHost<GameHost>(destroySectorRange, [this, &destroyList](GameHost* p)
		{
			if (_host != p)
			{
				destroyList.push_back(p);
			}
		});

	OnDestroyRequest(destroyList);

	//OnSectorLeave(destroySectorRange);

	_sector->EraseObject(this);

	list my{ _host };

	if (_host != nullptr)
	{
		ExecuteForEachObject<GameObject>(destroySectorRange,
			[this, &my](GameObject* go)
			{
				if (go == this)
				{
					return;
				}

				go->OnDestroyRequest(my);
			});
		_sector->hosts.erase(_host);
	}
	_sector = newSector;
	position = { x, y };
	_sector->InsertObject(this);
	if (_host != nullptr)
	{
		ExecuteForEachObject<GameObject>(spawnSectorRange,
			[this, &my](GameObject* go)
			{
				if (go == this)
				{
					return;
				}

				go->OnSpawnRequest(my);
			});
		_sector->hosts.insert(_host);
	}
	list<GameHost*> spawnList;

	ExecuteForEachHost<GameHost>(spawnSectorRange, [this, &spawnList](GameHost* p)
		{
			if (_host != p)
			{
				spawnList.push_back(p);
			}
		});

	OnSpawnRequest(spawnList);

	//OnSectorEnter(spawnSectorRange);
}

void GameObject::MoveTowards(const Eigen::Vector2<float32>& target, float32 maxDistance)
{
	Eigen::Vector2<float32> dir = target - position;
	float32 distance = dir.norm();
	if (distance == 0.0f)
	{
		return;
	}

	dir = dir / distance;

	float moveDistance = fmin(maxDistance, distance);

	Position newPos = position + dir * moveDistance;
	MovePosition(newPos.x(), newPos.y());
}

void GameObject::LookAt(const Eigen::Vector2<float32>& target)
{
	Eigen::Vector2<float32> dir = target - position;
	if (dir == Eigen::Vector2<float32>::Zero())
	{
		return;
	}

	yaw = atan2(dir.x(), dir.y()) * (180.0f / numbers::pi_v<float32>);
}

void FixedObject::SendPacket(uint64 sessionId, Packet pkt)
{
	_owner->SendPacket(sessionId, pkt);
}

float32 FixedObject::DeltaTime()
{
	return _owner->DeltaTime();
}

void FixedObject::DestroyObject(GameObject* object)
{
	_owner->DestroyObject(object);
}

void FixedObject::Invoke(function<void()>&& func, DWORD afterTick)
{
	_owner->Invoke(std::move(func), afterTick);
}

void FixedObject::Invoke(function<void()>&& func, float32 afterTime)
{
	_owner->Invoke(std::move(func), afterTime);
}
