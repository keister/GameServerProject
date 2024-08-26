#include "stdafx.h"
#include "GameHost.h"
#include "GameObject.h"

uint64 GameHost::AttachObject(GameObject* object)
{
	(object)->_host = this;
	_attachObject = object;

	object->_sector->hosts.insert(this);

	return 0;
}

void GameHost::DetachObject()
{
	if (_attachObject == nullptr)
		return;

	_attachObject->_sector->hosts.erase(this);
	_attachObject->_host = nullptr;
}


GameHost::~GameHost()
{
	
}
