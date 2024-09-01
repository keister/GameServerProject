#include "stdafx.h"
#include "GameHost.h"
#include "GameObject.h"


/// @brief 유저의 위치는 한 게임오브젝트에 투영하여 결정된다.
/// @param object 투영할 오브젝트
/// @return 
uint64 game::GameHost::AttachObject(GameObject* object)
{
	(object)->_host = this;
	_attachObject = object;

	object->_sector->hosts.insert(this);

	return 0;
}

void game::GameHost::DetachObject()
{
	if (_attachObject == nullptr)
		return;

	_attachObject->_sector->hosts.erase(this);
	_attachObject->_host = nullptr;
}


game::GameHost::~GameHost()
{
	
}
