#include "stdafx.h"
#include "GameHost.h"
#include "GameObject.h"


/// @brief ������ ��ġ�� �� ���ӿ�����Ʈ�� �����Ͽ� �����ȴ�.
/// @param object ������ ������Ʈ
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
