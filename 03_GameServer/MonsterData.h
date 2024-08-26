#pragma once
#include <codecvt>

struct MonsterInfo
{
	uint64 id;
	int32 level;
	int32 hp;
	int32 attackDamage;
	int32 speed;
	float32 attackRange;
	float32 attackSpeed;
	float32 castTime;
	int32 giveExp;
};

class MonsterData
{
	inline static Lock _lock;
	static MonsterData& Instance()
	{
		static MonsterData* monster = nullptr;

		if (monster == nullptr)
		{
			WRITE_LOCK(_lock);
			if (monster == nullptr)
			{
				monster = new MonsterData();
			}
		}

		return *monster;
	}

	MonsterData()
	{
		std::ifstream ifs("./monster.json");
		json parseData = json::parse(ifs);

		for (auto j : parseData["monsters"])
		{
			uint64 id = j["id"].get<uint64>();
			auto ret = _map.insert({ id, MonsterInfo{} });
			MonsterInfo& info = ret.first->second;
			info.id = id;
			info.level = j["level"];
			info.hp = j["hp"];
			info.attackDamage = j["attackDamage"];
			info.speed = j["speed"];
			info.attackRange = j["attackRange"];
			info.attackSpeed = j["attackSpeed"];
			info.castTime = j["castTime"];
			info.giveExp = j["exp"];
		}
	}



	unordered_map<uint64, MonsterInfo> _map;

public:
	static const MonsterInfo& Get(uint64 id)
	{
		MonsterData& data = Instance();
		auto ret = data._map.find(id);

		if (ret == data._map.end())
		{
			CRASH();
		}

		return ret->second;
	}
};

