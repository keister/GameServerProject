#pragma once


class MapData;

class MapManager
{
public:
	static MapManager& Instance()
	{
		static MapManager* instance = nullptr;
		if (instance == nullptr)
		{
			WRITE_LOCK(lock);
			if (instance == nullptr)
			{
				instance = new MapManager;
			}
		}
	}


	void Load(const wstring& name);
	

private:
	unordered_map<uint64, MapData*> _maps;

	inline static Lock lock;

};

