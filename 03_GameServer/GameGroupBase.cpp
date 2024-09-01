#include "stdafx.h"
#include "GameServer.h"
#include "GameGroupBase.h"

#include "PathReceiver.h"
#include "Player.h"
#include "Route.h"


void game::GameGroupBase::SetServer(GameServer* server)
{
	_server = server;
}

bool game::GameGroupBase::CreateMap(const char* fileName, int32 sectorWidth, int32 sectorHeight)
{
	if (_map != nullptr)
		return false;


	_map = new Map(this, fileName, sectorWidth, sectorHeight);

	return true;
}

void game::GameGroupBase::ReleaseFixedObject(FixedObject* object)
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

void game::GameGroupBase::PathFindingCompletionRoutine(GameObject* gameObject, uint64 objectId, uint64 execCount, Route& route)
{
	/// 요청한 객체가 파괴되었으면 무시
	if (gameObject->ObjectId() != objectId)
	{
		return;
	}

	/// 가장 최신의 결과만 반영함
	if (gameObject->_foundPath.ExecutionCount() != execCount)
	{
		return;
	}

	gameObject->GetPathReciever().GetRoute().Move(route);
	gameObject->GetPathReciever().ResetCurrentTargetPosition();
	gameObject->OnPathFindingCompletion();
}

void game::GameGroupBase::ReleaseObject(GameObject* object)
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

game::Player* game::GameGroupBase::FindPlayer(uint64 sessionId)
{
	auto findIt = _players.find(sessionId);

	if (findIt == _players.end())
	{
		return nullptr;
	}

	return findIt->second;
}

void game::GameGroupBase::InsertPlayer(Player* player)
{
	_players.insert({ player->SessionId(), player });
}

void game::GameGroupBase::DeletePlayer(uint64 sessionId)
{
	_players.erase(sessionId);
}


void game::GameGroupBase::SendPacket(uint64 sessionId, Packet pkt)
{
	GroupBase::SendPacket(sessionId, pkt);
}

void game::GameGroupBase::SendPacket(Sector* sector, int32 sectorRange, Packet pkt)
{
	if (_map == nullptr)
	{
		return;
	}

	_map->SendPacket(sector, sectorRange, pkt);
}

void game::GameGroupBase::SendPacket(Sector* sector, int32 sectorRange, Packet pkt, uint64 except)
{
	if (_map == nullptr)
	{
		return;
	}

	_map->SendPacket(sector, sectorRange, pkt, except);
}

void game::GameGroupBase::SendPacket(Sector* sector, int32 sectorRange, Packet pkt, list<uint64>& exceptSession)
{
	if (_map == nullptr)
	{
		return;
	}

	_map->SendPacket(sector, sectorRange, pkt, exceptSession);
}

void game::GameGroupBase::DestroyObject(GameObject* object)
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

void game::GameGroupBase::DestroyFixedObject(FixedObject* object)
{
	object->_isDestroy = true;
	_removefixedObjects.push_back(object);
}

/// @brief 일정 시간이후에 함수호출을 예약함. 해당오브젝트가 삭제되면 무시된다.
void game::GameGroupBase::Invoke(BaseObject* object, function<void()>&& func, DWORD afterTick)
{
	_timerEventQueue.push(TimerEvent{ CurrentTick() + afterTick, func, object, object->ObjectId() });
}

/// @brief 일정 시간이후에 함수호출을 예약함. 해당오브젝트가 삭제되면 무시된다.
void game::GameGroupBase::Invoke(BaseObject* object, function<void()>&& func, float32 afterTime)
{
	_timerEventQueue.push(TimerEvent{ CurrentTick() + (int32)(afterTime * 1000), func, object, object->ObjectId() });
}

/// @brief 현재 좌표를 기준으로 속한 섹터를 찾는다.
game::Sector* game::GameGroupBase::FindSectorByPostion(float32 x, float32 y)
{
	if (_map == nullptr)
	{
		return nullptr;
	}

	int32 sectorY = y / _map->SectorHeight();
	int32 sectorX = x / _map->SectorWidth();

	return _map->GetSector(sectorY, sectorX);
}

void game::GameGroupBase::OnEnter(uint64 sessionId)
{
	Player* player = _server->GetHost<Player>(sessionId);
	InsertPlayer(player);
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

void game::GameGroupBase::OnLeave(uint64 sessionId)
{
	Player* player = FindPlayer(sessionId);

	if (player == nullptr)
	{
		CRASH();
	}

	OnPlayerLeave(*player);
	DeletePlayer(sessionId);
}

void game::GameGroupBase::UpdateFrame()
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
void game::GameGroupBase::process_timer_event()
{
	while (_timerEventQueue.size() > 0)
	{
		const TimerEvent& event = _timerEventQueue.top();

		if (_timerEventQueue.top().reservedTick < CurrentTick())
		{
			if (event.gameObject->_isDestroy)
			{
				_timerEventQueue.pop();
				continue;
			}

			if (event.gameObject->ObjectId() != event.objectId)
			{
				_timerEventQueue.pop();
				continue;
			}


			_timerEventQueue.top()();
			_timerEventQueue.pop();
		}
		else
		{
			break;
		}
	}
}