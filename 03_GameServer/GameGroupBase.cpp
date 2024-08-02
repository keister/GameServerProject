#include "stdafx.h"
#include "GameServer.h"
#include "GameGroupBase.h"
#include "Player.h"



void GameGroupBase::SetServer(GameServer* server)
{
	_server = server;
}

bool GameGroupBase::CreateMap(int32 width, int32 height, int32 sectorWidth, int32 sectorHeight)
{
	if (_map != nullptr)
		return false;


	_map = new Map(this, width, height, sectorWidth, sectorHeight);

	return true;
}

void GameGroupBase::ReleaseFixedObject(FixedObject* object)
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

void GameGroupBase::ReleaseObject(GameObject* object)
{
	object->_sector->EraseObject(object);

	if (object->_doPooling == true)
	{
		object->_freeFunc(object);
	}
	else
	{
		delete object;
	}
}

Player* GameGroupBase::find_player(uint64 sessionId)
{
	auto findIt = _players.find(sessionId);

	if (findIt == _players.end())
	{
		return nullptr;
	}

	return findIt->second;
}

void GameGroupBase::insert_player(Player* player)
{
	_players.insert({ player->SessionId(), player });
}

void GameGroupBase::delete_player(uint64 sessionId)
{
	_players.erase(sessionId);
}


void GameGroupBase::SendPacket(uint64 sessionId, Packet pkt)
{
	GroupBase::SendPacket(sessionId, pkt);
}

void GameGroupBase::SendPacket(Sector* sector, int32 sectorRange, Packet pkt)
{
	if (_map == nullptr)
	{
		return;
	}

	_map->SendPacket(sector, sectorRange, pkt);
}

void GameGroupBase::SendPacket(Sector* sector, int32 sectorRange, Packet pkt, uint64 except)
{
	if (_map == nullptr)
	{
		return;
	}

	_map->SendPacket(sector, sectorRange, pkt, except);
}

void GameGroupBase::SendPacket(Sector* sector, int32 sectorRange, Packet pkt, list<uint64>& exceptSession)
{
	if (_map == nullptr)
	{
		return;
	}

	_map->SendPacket(sector, sectorRange, pkt, exceptSession);
}

void GameGroupBase::DestroyObject(GameObject* object)
{
	object->_isDestroy = true;
	list<GameHost*> other;
	object->ExecuteForEachHost<GameHost>(AROUND,
		[&other](GameHost* ho)
		{
			other.push_back(ho);
		});

	object->OnDestroyRequest(other);
	_removeObjects.push_back(object);
}

void GameGroupBase::DestroyFixedObject(FixedObject* object)
{
	object->_isDestroy = true;
	_removefixedObjects.push_back(object);
}

void GameGroupBase::Invoke(function<void()>&& func, DWORD afterTick)
{
	_timerEventQueue.push(TimerEvent{ CurrentTick() + afterTick, func });
}

void GameGroupBase::Invoke(function<void()>&& func, float32 afterTime)
{
	_timerEventQueue.push(TimerEvent{ CurrentTick() + (int32)(afterTime * 1000), func });
}

Sector* GameGroupBase::FindSectorByPostion(float32 x, float32 y)
{
	if (_map == nullptr)
	{
		return nullptr;
	}

	int32 sectorY = y / _map->SectorHeight();
	int32 sectorX = x / _map->SectorWidth();

	return _map->GetSector(sectorY, sectorX);
}

void GameGroupBase::OnEnter(uint64 sessionId)
{
	Player* player = _server->GetHost<Player>(sessionId);
	insert_player(player);
	OnPlayerEnter(*player);

	list<GameHost*> my{ player };

	GameObject* playerObject = player->GetGameObject();

	if (playerObject == nullptr)
		return;

	playerObject->ExecuteForEachObject<GameObject>(AROUND,
		[&player, &my](GameObject* go)
		{
			if (player->GetGameObject()->_objectId == go->_objectId)
			{
				return;
			}

			go->OnSpawnRequest(my);
		});

}

void GameGroupBase::OnLeave(uint64 sessionId)
{
	Player* player = find_player(sessionId);

	if (player == nullptr)
	{
		CRASH();
	}

	OnPlayerLeave(*player);
	delete_player(sessionId);
}

void GameGroupBase::UpdateFrame()
{
	process_timer_event();

	for (FixedObject* obj : _fixedObjects)
	{
		if (obj->_isDestroy == false)
		{
			obj->OnUpdate();
		}
	}

	for (GameObject* obj : _gameObjects)
	{
		if (obj->_isDestroy == false)
		{
			obj->OnUpdate();
		}
	}

	for (FixedObject* obj : _removefixedObjects)
	{
		_fixedObjects.erase(obj);
		ReleaseFixedObject(obj);
	}

	_removefixedObjects.clear();


	for (GameObject* obj : _removeObjects)
	{
		_gameObjects.erase(obj);
		ReleaseObject(obj);
	}

	_removeObjects.clear();


}
void GameGroupBase::process_timer_event()
{
	while (_timerEventQueue.size() > 0)
	{
		if (_timerEventQueue.top().reservedTick < CurrentTick())
		{
			_timerEventQueue.top()();
			_timerEventQueue.pop();
		}
		else
		{
			break;
		}
	}
}