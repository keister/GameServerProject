#pragma once
#include "ObjectPool.h"

namespace netlib
{

	template <typename T>
	class LockFreeStack
	{
		struct Node
		{
			T data;
			Node* next;
		};

	public:
		LockFreeStack();
		~LockFreeStack();

		uint64 Size() { return _size; }

		void Push(T data);
		void Pop(T& data);

	private:
		static Node* remove_id_from_address(void* ptr)
		{
			return reinterpret_cast<Node*>(reinterpret_cast<uint64>(ptr) & ADDRESS_MASK);
		}
		static Node* merge_id_into_address(void* ptr, uint64 id)
		{
			return reinterpret_cast<Node*>(reinterpret_cast<uint64>(ptr) | (id << (64 - NUM_OF_ID_BITS)));
		}

	private:
		using Allocator = ObjectPool<Node, false>;

		Node* _head;

		uint64		_size;
		uint64		_id;
		Allocator	_allocator;
	};
}

#include "LockFreeStack.hpp"
