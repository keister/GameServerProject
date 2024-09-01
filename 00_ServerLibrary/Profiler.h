#pragma once
#ifdef PROFILE
#define __AUTO_PROFILER_NAME CONCAT(__profiler, __COUNTER__)
#define SCOPE_PROFILE(TagName) \
		netlib::ScopeProfiler __AUTO_PROFILER_NAME(CONCAT(L, TagName))

#define PROFILE_BEGIN(TagName)				netlib::Profiler::Begin(TagName)
#define PROFILE_END(TagName)				do										\
											{										\
												LARGE_INTEGER endTime;				\
												QueryPerformanceCounter(&endTime);	\
												Profiler::End(TagName, endTime);	\
											}										\
											while(false)

#define PROFILE_DATA_OUT_NAME(FileName)		netlib::Profiler::DataOut(FileName)
#define PROFILE_DATA_OUT()					netlib::Profiler::DataOut()
#define PROFILE_RESET()						netlib::Profiler::Reset()
#define PROFILE_SAVE(key)					do											\
											{											\
												if (GetAsyncKeyState(key))				\
												{										\
													PROFILE_DATA_OUT();					\
													wcout << L"PROFILE SAVED\n";		\
												}										\
											} while (false);
											
#else
#define SCOPE_PROFILE(TagName)
#define PROFILE_BEGIN(TagName)
#define PROFILE_END(TagName)
#define PROFILE_DATA_OUT_NAME(FileName)
#define PROFILE_DATA_OUT()
#define PROFILE_RESET()
#define PROFILE_SAVE(key)
#endif
#include "Lock.h"

namespace netlib
{
	class Profiler
	{
		enum : int64
		{
			NULLTIME = INT64_MAX,
			SECOND_PER_MICRO_SECOND = 1'000'000,
		};

		struct Sample
		{
			bool			useFlag{};
			wstring_view	name;
			LARGE_INTEGER	startTime{};
			uint64			totalTime{};
			uint64			min[2]{};
			uint64			max[2]{};
			uint64			callCount{};
		};

	public:
		static void Begin(wstring_view name);
		static void End(wstring_view name, LARGE_INTEGER endTime);
		static void DataOut(wstring_view fileName);
		static void DataOut();
		static void Reset();
		static void Attach();

	private:
		static LARGE_INTEGER	get_performance_frequency();
		static Sample* find_sample(wstring_view name);
		static Sample* get_empty_sample();

	private:
		inline static Lock _sampleMapLock;
		inline static unordered_map<uint32, Sample*> _sampleMap;

		thread_local
			inline static Sample _profileSamples[100];
		thread_local inline static bool _isConstruct = false;
	};

	class ScopeProfiler
	{
	public:
		ScopeProfiler(wstring_view name)
			: _name(name)
		{
			PROFILE_BEGIN(name);
		}

		~ScopeProfiler()
		{
			PROFILE_END(_name);
		}


	private:
		wstring_view _name;
	};
}