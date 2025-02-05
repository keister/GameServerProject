#pragma once
class AppSettings
{
	inline static netlib::Lock _lock;

	static AppSettings& Instance()
	{
		static AppSettings* instance = nullptr;

		if (instance == nullptr)
		{
			WRITE_LOCK(_lock);
			if (instance == nullptr)
			{
				instance = new AppSettings;
			}
		}

		return *instance;
	}

	AppSettings()
	{
		std::ifstream ifs("./appsettings.json");
		_json = json::parse(ifs);
	}

	inline static json _json;

public:
	static const json& GetSection(const char* key)
	{
		try
		{
			return Instance()._json[key];
		}
		catch (json::exception err)
		{
			const char* errStr = err.what();
			wstring errString(errStr, errStr + strlen(errStr));
			LOG_ERR(L"Parse", L"%s", errString.c_str());
		}
	}
};

