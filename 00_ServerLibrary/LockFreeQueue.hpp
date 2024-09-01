#pragma once
#include "LockFreeQueue.h"

template <typename T>
netlib::LockFreeQueue<T>::LockFreeQueue()
	: _id(0)
	, _size(0)
{
	_head = _allocator.Alloc();
	_head->next = nullptr;
	_head = merge_id_into_address(_head, _id);
	_tail = _head;
	_size = 0;
	_id = 0;
}

template <typename T>
netlib::LockFreeQueue<T>::LockFreeQueue::~LockFreeQueue()
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
void netlib::LockFreeQueue<T>::Push(T data)
{
	Node* node = _allocator.Alloc();
	node->data = data;
	node->next = nullptr;

	const uint64 key = InterlockedIncrement64(reinterpret_cast<LONG64*>(&_id));
	node = merge_id_into_address(node, key);

	for (;;)
	{
		Node* tail = _tail;
		Node* rawTail = remove_id_from_address(tail);
		Node* next = rawTail->next;

		/// next가 nullptr이 아니라면 2번CAS가 실패했을 경우이므로, 한칸 땡겨준다.
		if (next != nullptr)
		{
			InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_tail), next, tail);
			continue;
		}

		
		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&rawTail->next), node, next) == next)
		{
			/// 2번 CAS가 실패하는 상황
			///	두 스레드가 같은  tail(0x12345678이라고 가정)을 들고있는 상황(1번 CAS 실행전에 한스레드가 멈춰있음).
			///	한 스레드가 pop, push를 반복,  이 과정이에서 tail이었던 0x12345678이 pop되고 다시 해당 주소로 push됨
			///	여기서 다른 스레드가 1번 CAS실행함
			///	그러면 둘다 next는 nullptr이므로 1번 CAS는 통과하나, 서로 tail값은 다르므로 2번 CAS는 실패한다.
			InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_tail), node, tail);
			break;
		}
	}

	InterlockedIncrement64(&_size);
}

template <typename T>
bool netlib::LockFreeQueue<T>::Pop(T& data)
{
	for (;;)
	{
		Node* head = _head;
		Node* tail = _tail;
		Node* rawHead = remove_id_from_address(head);
		Node* next = rawHead->next;

		if (next == nullptr)
		{
			return false;
		}

		/// 발생되는 현상. 무한 뺑뺑이 2번CAS실패하는 상황이랑 연계됨.
		///	두 쓰레드가 같은 tail을 바라보는 상황에서 1번 쓰레드가 pop, push를 반복하여
		///	2번 쓰레드가 1번 CAS는성공하였지만, 2번 CAS는 실패했다고 가정한ㄷ,
		///	이 이후로, 서로 pop만 하게되면 tail을 끝으로 옮겨줄 주체가 없다.
		///	이렇게 pop이 지속되다가 head == tail인 상황이 발생하면(실제로는 비어있지 않음)
		///	head->next는 nullptr이 아니므로 정상적으로 pop이 진행되고 head의 위치가 옮겨지겠지만
		///	tail의 위치는 그대로이다. 이렇게되면 head가 tail을 역전해버리고
		///	tail이 가리키고 있는 주소는 해제되었으므로 다음에 재할당되면 같은 주소가 tail에 또붙는다.
		///	이렇게되면 push에서 무한 뺑뺑이가 돌게된다.
		///	head == tail의 상황에서 tail->next가 null이 아니지만, pop에서는 tail의 상황을 모르니
		///	head만 판단하여서 head->next로 head를 옮겨버린다. 이렇게되면, tail은 움직이지 않고,
		///	head만 뒤로 움직인다.
		/// tail은 방금 삭제한 더미 head를 들고있는 상태이며, 다음 Push에서 tail과 같은 주소의 노드가
		///	할당되고, head->next는 null이고, tail->next는 tail과 같은 상황이 나타난다.
		if (head == tail)
		{
			InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_tail), next, tail);
			continue;
		}


		T value = remove_id_from_address(next)->data;
		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_head), next, head) == head)
		{
			// 여기서 멈춤(head바뀜)
			// 다른 스레드가 해당 head를 삭제하고
			// 다시 할당함. 해당주소는 다른노드의 주소를 가지고 있음.
			// 따라서 이상한 데이터 뽑힘
			// 그래서 CAS하기 전에 데이터를 미리 백업해놓아야한다.
			data = value;
			_allocator.Free(rawHead);

			break;
		}
	}

	InterlockedDecrement64(&_size);
	return true;
}

