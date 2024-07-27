#include "stdafx.h"
#include "GameServer.h"
#include "GameGroupBase.h"
#include "Player.h"



void GameGroupBase::SetServer(GameServer* server)
{
	_server = server;
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
	_players.insert({ player->sessionId, player });
}

void GameGroupBase::delete_player(uint64 sessionId)
{
	_players.erase(sessionId);
}

void GameGroupBase::Invoke(function<void()>&& func, DWORD afterTick)
{
	_timerEventQueue.push(TimerEvent{ CurrentTick() + afterTick, func });
}

void GameGroupBase::Invoke(function<void()>&& func, float32 afterTime)
{
	_timerEventQueue.push(TimerEvent{ CurrentTick() + (int32)(afterTime * 1000), func });
}

void GameGroupBase::OnEnter(uint64 sessionId)
{
	Player* player = _server->FindPlayer(sessionId);
	insert_player(player);
}

void GameGroupBase::OnLeave(uint64 sessionId)
{
	delete_player(sessionId);
}

void GameGroupBase::UpdateFrame()
{
	process_timer_event();


	for (GameObject* obj : _gameObjects)
	{
		obj->OnUpdate();
	}

	for (GameObject* obj : _removeObjects)
	{
		_gameObjects.erase(obj);
		ReleaseObject(obj);
	}
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