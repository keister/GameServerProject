#pragma once
#include "LockFreeStack.h"

template <typename T>
netlib::LockFreeStack<T>::LockFreeStack()
	: _head(nullptr)
	, _size(0)
	, _id(0)
	, _allocator()
{
}

template <typename T>
netlib::LockFreeStack<T>::~LockFreeStack()
{
	Node* node = remove_id_from_address(_head);

	while (node != nullptr)
	{
		Node* next = remove_id_from_address(node->next);
		_allocator.Free(node);
		node = next;
	}
}

template <typename T>
void netlib::LockFreeStack<T>::Push(T data)
{
	Node* newNode = _allocator.Alloc();
	newNode->data = data;

	const uint64 id = InterlockedIncrement64(reinterpret_cast<LONG64*>(&_id));

	Node* newTopPtr = merge_id_into_address(newNode, id);

	for (;;)
	{
		Node* top = _head;
		newNode->next = top;

		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_head), newTopPtr, top) == top)
		{
			InterlockedIncrement64(reinterpret_cast<LONG64*>(&_size));
			break;
		}
	}
}

template <typename T>
void netlib::LockFreeStack<T>::Pop(T& data)
{
	Node* nodePtr;

	for (;;)
	{
		Node* deleteNode = _head;
		if (deleteNode == nullptr)
		{
			CRASH("STACK EMPTY");
		}

		nodePtr = remove_id_from_address(deleteNode);

		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_head), nodePtr->next, deleteNode) == deleteNode)
		{
			InterlockedDecrement64(reinterpret_cast<LONG64*>(&_size));
			break;
		}
	}

	data = nodePtr->data;

	_allocator.Free(nodePtr);
}