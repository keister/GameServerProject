#pragma once
#include "ObjectPool.h"


template <typename T, bool IsConstructorCalled>
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/// @brief Constructor
///
/// @param initBlocks	�ʱ� �� ����
/// @param args			�����ڿ� �� ���ڸ��
///
//-------------------------------------------------------------------------------
template <typename... Args>
netlib::ObjectPool<T, IsConstructorCalled>::ObjectPool(int32 initBlocks, Args&&... args) :
	_head(nullptr), _capacity(initBlocks), _useCount(0), _id(0)
{
	//-----------------------------------------------------------------------------------------------
	/// ���� OS���� UserSpace �ּҷ� 64��Ʈ�� ���δ� ������� �ʱ� ������, �����Ϳ� �ش� ��Ʈ��
	///	ID�� ���ܳ��� �� �ִ�. �ش� ������� ABA������ �ذ��� ���ε�, ���� ���������� 17��Ʈ�� ����
	///	�ִ�. ������ OS ������ �ٲ�� �ش� ��Ʈ�� ������ �޶��� �� �����Ƿ�, �ش� �ڵ忡�� CRASH��
	///	���ٸ� NUM_OF_ID_BITS�� ���� ���� OS��翡 �°� �����ؾ� �Ѵ�.
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	const uint64 userSpaceMaximumAddress = reinterpret_cast<uint64>(info.lpMaximumApplicationAddress);

	CRASH_ASSERT((userSpaceMaximumAddress >> (64 - NUM_OF_ID_BITS)) == 0);
	//-----------------------------------------------------------------------------------------------

	InitializeCriticalSection(&cs);

	for (int32 i = 0; i < initBlocks; i++)
	{
		BlockNode* ptr = reinterpret_cast<BlockNode*>(new BYTE[sizeof(BlockNode)]);

		// ������ ȣ��ɼ��� ����������, ��ó�� ������ �� �ѹ��� �����ڰ� ȣ��Ǿ�� �Ѵ�.
		if constexpr (IsConstructorCalled == false)
		{
			new(ptr)T(args...);
		}

#ifdef MEMORY_CHECK
		ptr->paddingFront = this;
		ptr->paddingRear = this;
#endif

		// node push
		const uint64 id = InterlockedIncrement64(reinterpret_cast<LONG64*>(&_id));

		ptr->next = _head;
		_head = merge_id_into_address(_head, id);
		
	}

}



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/// @brief ���� FreeList�� �ִ� ���鸸 ������� �޸� ������ �����Ѵ�.
///
//-------------------------------------------------------------------------------
template <typename T, bool IsConstructorCalled>
netlib::ObjectPool<T, IsConstructorCalled>::~ObjectPool()
{
	BlockNode* node = remove_id_from_address(_head);

	while (node != nullptr)
	{
		BlockNode* next = remove_id_from_address(node->next);

		if constexpr (IsConstructorCalled == true)
		{
			::free(node);
		}
		else
		{
			delete node;
		}

		node = next;
	}
}



template <typename T, bool IsConstructorCalled>
//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/// @brief �޸𸮸� Pool���� �����´�. ������� �Ҵ��Ѵ�.
///
/// @param args		�����ڿ� �� ���ڸ��
/// @return			�Ҵ�� ������
///
//-------------------------------------------------------------------------------
template <typename... Args>
T* netlib::ObjectPool<T, IsConstructorCalled>::Alloc(Args&&... args)
{
	BlockNode* ptr;

	// pop node

	for(;;)
	{
		BlockNode* returnNode = _head;
		if (returnNode == nullptr)
		{
			ptr = nullptr;
			break;
		}

		ptr = remove_id_from_address(returnNode);

		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_head), ptr->next, returnNode) == returnNode)
		{
			break;
		}
	}


	if (ptr == nullptr) /* Pool�� ����ִٸ� ��带 ���� �����Ҵ�*/
	{
		ptr = reinterpret_cast<BlockNode*>(new BYTE[sizeof(BlockNode)]);

#ifdef MEMORY_CHECK
		ptr->paddingFront = this;
		ptr->paddingRear = reinterpret_cast<BYTE*>(this) + 1;
		ptr = reinterpret_cast<BlockNode*>(reinterpret_cast<BYTE*>(ptr) + sizeof(void*));
#endif

		new(ptr)T(args...); // ������ ȣ��

		InterlockedIncrement(reinterpret_cast<LONG*>(&_capacity));
	}
	else
	{
#ifdef MEMORY_CHECK
		ptr->paddingFront = this;
		ptr->paddingRear = reinterpret_cast<BYTE*>(ptr->paddingRear) + 1;
		ptr = reinterpret_cast<BlockNode*>(reinterpret_cast<BYTE*>(ptr) + sizeof(void*));
#endif

		if constexpr (IsConstructorCalled == true)
		{
			new(ptr)T(args...); // ������ ȣ��
		}

	}

	InterlockedIncrement(reinterpret_cast<LONG*>(&_useCount));

	return reinterpret_cast<T*>(ptr);
}



//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
/// @brief �޸𸮸� Pool�� ��ȯ�Ѵ�.
///
/// @param ptr ������ �޸� �����ּ�
///
//-------------------------------------------------------------------------------
template <typename T, bool IsConstructorCalled>
void netlib::ObjectPool<T, IsConstructorCalled>::Free(T* ptr)
{
	if constexpr (IsConstructorCalled == true)
	{
		ptr->~T(); // �Ҹ��� ȣ��
	}

	BlockNode* block = reinterpret_cast<BlockNode*>(ptr);
#ifdef MEMORY_CHECK
	block = reinterpret_cast<BlockNode*>(reinterpret_cast<BYTE*>(block) - sizeof(void*));

	// padding�� �ջ�Ǿ��ٸ�, CRASH
	CRASH_ASSERT(block->paddingFront == this && block->paddingRear == reinterpret_cast<BYTE*>(this) + 1)

	block->paddingRear = reinterpret_cast<BYTE*>(block->paddingRear) - 1;
#endif


	// push node
	const uint64 id = InterlockedIncrement64(reinterpret_cast<LONG64*>(&_id));

	BlockNode* newTopPtr = merge_id_into_address(block, id);
	for (;;)
	{
		BlockNode* top = _head;
		block->next = top;
		if (InterlockedCompareExchangePointer(reinterpret_cast<PVOID*>(&_head), newTopPtr, top) == top)
		{
			InterlockedDecrement(reinterpret_cast<LONG*>(&_useCount));
			break;
		}
	}
	
}
