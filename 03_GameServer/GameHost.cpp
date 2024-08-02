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


GameHost::~GameHost()
{
	
}
