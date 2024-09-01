#include "stdafx.h"
#include "Route.h"

game::Route::Route()
{
	_head = empty()._head;
	_tail = empty()._tail;
}

game::Route::~Route()
{
}

void game::Route::push_back(const Position& pos)
{
	if (IsEmpty())
	{
		CreateList();
	}

	RouteNode* node = AllocRouteNode();
	node->point = pos;
	node->prev = _tail->prev;
	_tail->prev->next = node;
	_tail->prev = node;
	node->next = _tail;

}

void game::Route::push_front(const Position& pos)
{
	if (IsEmpty())
	{
		CreateList();
	}

	RouteNode* node = AllocRouteNode();
	node->point = pos;
	node->next = _head->next;
	_head->next->prev = node;
	_head->next = node;
	node->prev = _head;

}

void game::Route::Destroy()
{
	if (IsEmpty())
	{
		return;
	}

	RouteNode* cur = _head;

	while (cur == nullptr)
	{
		RouteNode* next = cur->next;
		FreeRouteNode(cur);
		cur = next;
	}

	_head = empty()._head;
	_tail = empty()._tail;
}

void game::Route::Move(Route& route)
{
	if (!IsEmpty())
	{
		Destroy();
	}

	_head = route._head;
	_tail = route._tail;
	route._head = empty()._head;
	route._tail = empty()._tail;
}

void game::Route::CreateList()
{
	_head = AllocRouteNode();
	_tail = AllocRouteNode();
	_head->prev = nullptr;
	_head->next = _tail;
	_tail->next = nullptr;
	_tail->prev = _head;
}
