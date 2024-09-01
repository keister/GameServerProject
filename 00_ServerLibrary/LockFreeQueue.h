#pragma once
#include "ObjectPool.h"
#include "ObjectPoolTls.h"


namespace netlib
{

	template <typename T>
	class LockFreeQueue
	{
		struct Node
		{
			T		data;
			Node* next;
		};



	public:
		LockFreeQueue();
		~LockFreeQueue();
		void Push(T data);
		bool Pop(T& data);
		int64 Size() { return _size; }

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
		Node* _tail;

		uint64		_id;
		int64		_size;
		Allocator	_allocator;
	};
}

#include "LockFreeQueue.hpp"
