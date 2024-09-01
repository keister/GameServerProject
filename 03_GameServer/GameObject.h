#pragma once
#include "GlobalPool.h"
#include "InheritanceReflection.h"
#include "PathReceiver.h"

namespace netlib
{
	class Packet;
}

namespace game
{
	class Sector;

	/// @brief 해당 클래스를 상속받은 GameObject는 메모리 할당에 오브젝트 풀을 사용한다.
	class enable_object_pooling
	{

	};

	class BaseObject
	{
		using FreeFuncType = std::function<void(void*)>;
		friend class GameGroupBase;
	public:
		virtual ~BaseObject() {};
		uint64 ObjectId() const { return _objectId; }
		bool IsDestroy() const { return _isDestroy; }

		GameGroupBase* GetGroup() const { return _owner; }

	protected:
		virtual void OnUpdate() {}

	protected:
		GameGroupBase*	_owner;
		uint64			_objectId;
		FreeFuncType	_freeFunc;
		bool			_isDestroy = false;
		bool			_doPooling;
	};

	/// @brief 모든 게임내의 움직일 수 있는 개체들은 해당 클래스를 상속받아 구현해야한다.
	class GameObject : public BaseObject
	{
#pragma region Inheritance Reflection
	public:
		using RootType = void;
		using SuperType = RootType;
		using ThisType = GameObject;
		static TypeInfo* StaticInfo()
		{
			static TypeInfo info = TypeInfo(TypeInfoInitializer<ThisType>("GameObject"));

			return &info;
		}
		virtual const TypeInfo* GetTypeInfo() { return  _typeInfo; }
		inline static TypeInfo* _typeInfo = StaticInfo();
#pragma endregion

		friend class GameGroupBase;
		friend class GameHost;
		friend PathReceiver;
	public:
		GameObject() : _foundPath(this) {};
		~GameObject() override = default;

		void SendPacket(uint64 sessionId, Packet pkt);
		void SendPacket(int32 sectorRange, Packet pkt);
		void SendPacket(int32 sectorRange, Packet pkt, uint64 except);
		void SendPacket(int32 sectorRange, Packet pkt, list<uint64>& exceptSession);

		template <typename T>
		bool ExecuteForEachHost(int32 sectorRange, std::function<void(T*)>&& func);
		template <typename T>
		bool ExecuteForEachObject(int32 sectorRange, std::function<void(T*)>&& func);

		/// @brief 생성 요청 패킷을 받을때 동작할 가상함수
		/// @param sessionList 보낼 유저들의 세션아이디를 리스트로넘겨줌
		virtual void OnSpawnRequest(const list<GameHost*>& sessionList) {}

		/// @brief 파괴 요청 패킷을 받을때 동작할 가상함수
		/// @param sessionList 보낼 유저들의 세션아이디를 리스트로넘겨줌
		virtual void OnDestroyRequest(const list<GameHost*>& sessionList) {}

		/// @brief 길찾기가 완료되었을 때 동작할 가상함수
		virtual void OnPathFindingCompletion() {};
		void Invoke(function<void()>&& func, DWORD afterTick);
		void Invoke(function<void()>&& func, float32 afterTime);


		PathReceiver& GetPathReciever() { return _foundPath; }

	protected:
		float32 DeltaTime();
		template <typename T, typename ...Args>
		T* CreateObject(const Position& pos, Args&&... args);
		void DestroyObject(GameObject* object);
		void MovePosition(float32 x, float32 y);
		void MoveTowards(const Position& target, float32 maxDistance);
		void LookAt(const Position& target);

	public:
		Position		position;
		float32			yaw = 0.f;

	protected:
		GameHost*		_host = nullptr;

	private:
		Sector*			_sector = nullptr;
		PathReceiver	_foundPath;
	};

	/// @brief 게임에 필요한 개체이지만 위치가 형체가 없는 오브젝트들은 해당 클래스를 상속받아 구현한다.
	class FixedObject : public BaseObject
	{
		friend class GameGroupBase;

	public:
		FixedObject() = default;
		~FixedObject() override = default;

		void	SendPacket(uint64 sessionId, Packet pkt);
		float32 DeltaTime();
	protected:
		template <typename T, typename ...Args>
		T*		CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args);
		void	DestroyObject(GameObject* object);
		void	Invoke(function<void()>&& func, DWORD afterTick);
		void	Invoke(function<void()>&& func, float32 afterTime);

	};
}

#include "GameGroupBase.h"

template <typename T>
bool game::GameObject::ExecuteForEachHost(int32 sectorRange, std::function<void(T*)>&& func)
{
	return _owner->ExecuteForEachHost<T>(_sector, sectorRange, std::move(func));
}

template <typename T>
bool game::GameObject::ExecuteForEachObject(int32 sectorRange, std::function<void(T*)>&& func)
{
	return _owner->ExecuteForEachObject<T>(_sector, sectorRange, std::move(func));
}

template <typename T, typename ... Args>
T* game::GameObject::CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args)
{
	return _owner->CreateObject<T>(pos, args...);
}

template <typename T, typename ... Args>
T* game::FixedObject::CreateObject(const Eigen::Vector2<float32>& pos, Args&&... args)
{
	return _owner->CreateObject<T>(pos, args...);
}
