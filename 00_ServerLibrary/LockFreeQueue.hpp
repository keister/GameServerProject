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

		/// next�� nullptr�� �ƴ϶�� 2��CAS�� �������� ����̹Ƿ�, ��ĭ �����ش�.
		if (next != nullptr)
		{
			InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_tail), next, tail);
			continue;
		}

		
		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&rawTail->next), node, next) == next)
		{
			/// 2�� CAS�� �����ϴ� ��Ȳ
			///	�� �����尡 ����  tail(0x12345678�̶�� ����)�� ����ִ� ��Ȳ(1�� CAS �������� �ѽ����尡 ��������).
			///	�� �����尡 pop, push�� �ݺ�,  �� �����̿��� tail�̾��� 0x12345678�� pop�ǰ� �ٽ� �ش� �ּҷ� push��
			///	���⼭ �ٸ� �����尡 1�� CAS������
			///	�׷��� �Ѵ� next�� nullptr�̹Ƿ� 1�� CAS�� ����ϳ�, ���� tail���� �ٸ��Ƿ� 2�� CAS�� �����Ѵ�.
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

		/// �߻��Ǵ� ����. ���� ������ 2��CAS�����ϴ� ��Ȳ�̶� �����.
		///	�� �����尡 ���� tail�� �ٶ󺸴� ��Ȳ���� 1�� �����尡 pop, push�� �ݺ��Ͽ�
		///	2�� �����尡 1�� CAS�¼����Ͽ�����, 2�� CAS�� �����ߴٰ� �����Ѥ�,
		///	�� ���ķ�, ���� pop�� �ϰԵǸ� tail�� ������ �Ű��� ��ü�� ����.
		///	�̷��� pop�� ���ӵǴٰ� head == tail�� ��Ȳ�� �߻��ϸ�(�����δ� ������� ����)
		///	head->next�� nullptr�� �ƴϹǷ� ���������� pop�� ����ǰ� head�� ��ġ�� �Ű���������
		///	tail�� ��ġ�� �״���̴�. �̷��ԵǸ� head�� tail�� �����ع�����
		///	tail�� ����Ű�� �ִ� �ּҴ� �����Ǿ����Ƿ� ������ ���Ҵ�Ǹ� ���� �ּҰ� tail�� �Ǻٴ´�.
		///	�̷��ԵǸ� push���� ���� �����̰� ���Եȴ�.
		///	head == tail�� ��Ȳ���� tail->next�� null�� �ƴ�����, pop������ tail�� ��Ȳ�� �𸣴�
		///	head�� �Ǵ��Ͽ��� head->next�� head�� �Űܹ�����. �̷��ԵǸ�, tail�� �������� �ʰ�,
		///	head�� �ڷ� �����δ�.
		/// tail�� ��� ������ ���� head�� ����ִ� �����̸�, ���� Push���� tail�� ���� �ּ��� ��尡
		///	�Ҵ�ǰ�, head->next�� null�̰�, tail->next�� tail�� ���� ��Ȳ�� ��Ÿ����.
		if (head == tail)
		{
			InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_tail), next, tail);
			continue;
		}


		T value = remove_id_from_address(next)->data;
		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_head), next, head) == head)
		{
			// ���⼭ ����(head�ٲ�)
			// �ٸ� �����尡 �ش� head�� �����ϰ�
			// �ٽ� �Ҵ���. �ش��ּҴ� �ٸ������ �ּҸ� ������ ����.
			// ���� �̻��� ������ ����
			// �׷��� CAS�ϱ� ���� �����͸� �̸� ����س��ƾ��Ѵ�.
			data = value;
			_allocator.Free(rawHead);

			break;
		}
	}

	InterlockedDecrement64(&_size);
	return true;
}

