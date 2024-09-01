#pragma once
#include "GameHost.h"
#include "Packet.h"
#include "InheritanceReflection.h"
#include "PathFindingWorker.h"


namespace game
{
	enum class TileInfo : uint8;
	class GameGroupBase;
	class MapData;
	class GameObject;
	class GameHost;
	class Monster;
	class Sector;
	class GroupBase;
	class Player;

	using SectorList = list<Sector*>;

	enum SectorRange
	{
		UP = 0x01,
		LEFT_UP = 0x02,
		LEFT = 0x04,
		LEFT_DOWN = 0x08,
		DOWN = 0x10,
		RIGHT_DOWN = 0x20,
		RIGHT = 0x40,
		RIGHT_UP = 0x80,
		CENTER = 0x100,

		COL_CENTER = UP | CENTER | DOWN,
		COL_RIGHT = RIGHT_UP | RIGHT | RIGHT_DOWN,
		COL_LEFT = LEFT_UP | LEFT | LEFT_DOWN,

		ROW_UP = LEFT_UP | UP | RIGHT_UP,
		ROW_CENTER = LEFT | CENTER | RIGHT,
		ROW_DOWN = LEFT_DOWN | DOWN | RIGHT_DOWN,

		COL_LEFT_CENTER = COL_LEFT | COL_CENTER,
		COL_RIGHT_CENTER = COL_RIGHT | COL_CENTER,

		ROW_UP_CENTER = ROW_UP | ROW_CENTER,
		ROW_DOWN_CENTER = ROW_DOWN | ROW_CENTER,

		ARROW_LEFT_UP = COL_LEFT | ROW_UP,
		ARROW_RIGHT_UP = COL_RIGHT | ROW_UP,
		ARROW_LEFT_DOWN = COL_LEFT | ROW_DOWN,
		ARROW_RIGHT_DOWN = COL_RIGHT | ROW_DOWN,

		AROUND = 0x1FF,
	};

	class Sector
	{
	public:
		Point pos;
		unordered_set<GameHost*> hosts;

		template <typename T> requires is_base_of_v<GameHost, T>
		void ExecuteForEachHost(std::function<void(T*)>&& func);
		template <typename T> requires is_base_of_v<GameObject, T>
		void ExecuteForEachObject(std::function<void(T*)>&& func);

		void InsertObject(GameObject* obj);
		void EraseObject(GameObject* obj);

	private:
		using ClassifiedObjectByType = unordered_map<uint64, unordered_set<void*>>;

		ClassifiedObjectByType _classifiedObjects;
	};

	/// @brief 현재 섹터의 Host들에게 함수호출
	/// @tparam T	GameHost를 상속받은 타입
	/// @param func 호출할 함수
	template <typename T> requires is_base_of_v<GameHost, T>
	void Sector::ExecuteForEachHost(std::function<void(T*)>&& func)
	{
		for (GameHost* host : hosts)
		{
			func(static_cast<T*>(host));
		}
	}

	/// @brief 현재 섹터의 특정 Type만 지정하여 함수 호출
	/// @tparam T 함수를 실행할 GameObject를 상속받은 타입
	/// @param func 호출할 함수
	template <typename T> requires is_base_of_v<GameObject, T>
	void Sector::ExecuteForEachObject(std::function<void(T*)>&& func)
	{
		TypeInfo* info = T::StaticInfo();

		auto it = _classifiedObjects.find(info->id);

		if (it == _classifiedObjects.end())
		{
			return;
		}

		for (void* objs : it->second)
		{
			func((T*)objs);
		}
	}


	class Map
	{
	public:
		friend PathFindingWorker;

		static constexpr int32 dy[] = { 1, 1, 0, -1, -1, -1, 0, 1 };
		static constexpr int32 dx[] = { 0, -1, -1, -1, 0, 1, 1, 1 };

		struct SectorInfo
		{
			Sector* sector;
			vector<Sector*> aroundSectors{ 9, nullptr };
		};

		Map(GameGroupBase* group, const char* fileName, int32 sectorWidth, int32 sectorHeight);

		void SendPacket(Sector* sector, int32 sectorRange, Packet pkt, list<uint64>& except);
		void SendPacket(Sector* sector, int32 sectorRange, Packet pkt, uint64 except);
		void SendPacket(Sector* sector, int32 sectorRange, Packet pkt);

		MapData* GetData() const { return _mapData; }
		Sector*	GetSector(int32 y, int32 x) const { return _sectors[y][x].sector; }
		void	GetSectors(SectorList& list, int32 y, int32 x, int32 sectorRange);

		Sector* FindSectorByPosition(int32 y, int32 x);
		const vector<Sector*>& GetAroundSectors(Sector* sector);

		template <typename T> requires is_base_of_v<GameHost, T>
		void ExecuteForEachHost(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func);

		template <typename T> requires is_base_of_v<GameObject, T>
		void ExecuteForEachObject(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func);

		void RequestPathFinding(GameObject* gameObject, const Position& destination)
		{
			_pathFindingWorker.RequestPathFinding(gameObject, destination);
		}

		const TileInfo& GetTileInfo(const Position& pos);

		int32		SectorWidth() const { return _sectorWidth; }
		int32		SectorHeight() const { return _sectorHeight; }
		int32		SectorMaxY() const { return _sectorMaxY; }
		int32		SectorMaxX() const { return _sectorMaxX; }

		Position	GetRandomPostionInSection(int32 id);
		uint64		GetTotalPathFindingCount() { return _pathFindingWorker.GetTotalPathFindingCount(); }

	private:
		void		set_around_sectors(int32 y, int32 x);

	private:
		using SectorArray2D = vector<vector<SectorInfo>>;

		MapData*			_mapData;			///< 장애물, 몬스터 정보 등
		int32				_sectorWidth;
		int32				_sectorHeight;
		int32				_sectorMaxY;
		int32				_sectorMaxX;
		SectorArray2D		_sectors;			///< 맵에 속한 섹터들
		GameGroupBase*		_group;				///< 속한 그룹
		PathFindingWorker	_pathFindingWorker;	///< 길찾기 스레드
	};

	/// @brief 한 섹터로부터 인접한 섹터들에 있는 Host들에게 특정함수 호출\n
	///			비트연산을 통해 섹터 범위를 계산한다.
	/// @tparam T	GameHost를 상속받은 타입
	/// @param sector 기준 섹터
	/// @param sectorRange 섹터 범위
	/// @param func 호출할 함수
	template <typename T> requires is_base_of_v<GameHost, T>
	void Map::ExecuteForEachHost(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func)
	{
		vector<Sector*> around = _sectors[sector->pos.y][sector->pos.x].aroundSectors;
		int32 currentSector = 1;
		for (int32 idx = 0; idx < 9; idx++, currentSector <<= 1)
		{
			if ((sectorRange & currentSector) != 0)
			{
				Sector* targetSector = _sectors[sector->pos.y][sector->pos.x].aroundSectors[idx];
				if (targetSector == nullptr)
				{
					continue;
				}

				targetSector->ExecuteForEachHost<T>(std::move(func));
			}
		}
	}

	/// @brief 한 섹터로부터 인접한 섹터들에 있는 특정 타입의 GameObject들에게 특정함수 호출\n
	///			비트연산을 통해 섹터 범위를 계산한다.
	/// @tparam T	GameObject를 상속받은 타입
	/// @param sector 기준 섹터
	/// @param sectorRange 섹터 범위
	/// @param func 호출할 함수
	template <typename T> requires is_base_of_v<GameObject, T>
	void Map::ExecuteForEachObject(Sector* sector, int32 sectorRange, std::function<void(T*)>&& func)
	{
		vector<Sector*> around = _sectors[sector->pos.y][sector->pos.x].aroundSectors;
		int32 currentSector = 1;
		for (int32 idx = 0; idx < 9; idx++, currentSector <<= 1)
		{
			if ((sectorRange & currentSector) != 0)
			{
				Sector* targetSector = _sectors[sector->pos.y][sector->pos.x].aroundSectors[idx];
				if (targetSector == nullptr)
				{
					continue;
				}

				targetSector->ExecuteForEachObject<T>(std::move(func));
			}
		}
	}
}
