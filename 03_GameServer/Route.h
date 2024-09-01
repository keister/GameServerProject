#pragma once

namespace game
{
	struct RouteNode;

	struct RouteNode
	{
		Position point;
		RouteNode* next;
		RouteNode* prev;
	};

	class RouteIterator
	{
	public:
		RouteIterator(RouteNode* node = nullptr) : _node(node) {};
		RouteIterator(const RouteIterator& other) : _node(other._node) {}

		RouteIterator& operator++()
		{
			_node = _node->next;

			return *this;
		}

		RouteIterator operator++(int32)
		{
			RouteIterator tmp(_node);
			_node = _node->next;

			return tmp;
		}

		Position& operator*() const
		{
			return _node->point;
		}

		Position* operator->() { return &_node->point; }

		bool operator==(const RouteIterator& other) const
		{
			return _node == other._node;
		}

		bool operator!=(const RouteIterator& other) const
		{
			return _node != other._node;
		}

	private:
		RouteNode* _node;
	};

	/// @brief 길찾기 결과를 담는 자료구조. 링크드 리스트로 이루어져 있다.
	class Route
	{
		inline static ObjectPoolTls<RouteNode, false> _routeNodeAllocator;
	public:
		static RouteNode* AllocRouteNode() { return _routeNodeAllocator.Alloc(); }
		static void FreeRouteNode(RouteNode* node) { _routeNodeAllocator.Free(node); }


		static Route& empty()
		{
			static Route _empty{ AllocRouteNode(), AllocRouteNode() };

			return _empty;
		}

	public:
		Route();
		~Route();
		Route(const Route& other) : _head(other._head), _tail(other._tail) {}
		Route& operator=(const Route& other)
		{
			_head = other._head;
			_tail = other._tail;

			return *this;
		}

		RouteIterator begin() const { return { _head->next }; }
		RouteIterator end() const { return { _tail }; }
		void push_back(const Position& pos);
		void push_front(const Position& pos);
		void Destroy();
		bool IsEmpty() { return _head == empty()._head; }
		void Move(Route& route);

	private:
		Route(RouteNode* begin, RouteNode* end) : _head(begin), _tail(end)
		{
			_head->next = _tail;
			_tail->prev = _head;
		}
		void CreateList();

	private:
		RouteNode* _head;
		RouteNode* _tail;
	};
}




