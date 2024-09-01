#pragma once
#include <filesystem>

#define _LOG(type, level, fmt, ...) netlib::Logger::Log(type, level, fmt, __VA_ARGS__)

#define LOG_DBG(type, fmt, ...) _LOG(type, netlib::LogLevel::DBG, fmt, __VA_ARGS__)
#define LOG_SYS(type, fmt, ...) _LOG(type, netlib::LogLevel::SYS, fmt, __VA_ARGS__)
#define LOG_ERR(type, fmt, ...) _LOG(type, netlib::LogLevel::ERR, fmt, __VA_ARGS__)

#define SET_LOG_DIR(path) netlib::Logger::SetDirectory(path)
#define SET_LOG_LEVEL(lvl) netlib::Logger::level = lvl

namespace netlib
{
	enum class LogLevel
	{
		DBG,
		SYS,
		ERR,
	};

	class Logger
	{
	public:
		static void Log(const WCHAR* type, LogLevel logLevel, const WCHAR* fmt, ...);
		static void SetDirectory(std::wstring_view path) { _path = path; }
		static LogLevel GetLevel() { return level; }

		inline static LogLevel level = LogLevel::DBG;
	private:
		inline static std::wstring _typeStrings[3] = { L"DBG", L"SYS", L"ERR" };
		inline static int32 _logCounter = 0;
		inline static std::filesystem::path _path{ L"./log" };
	};
}

