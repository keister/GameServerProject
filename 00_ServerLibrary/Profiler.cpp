#include "stdafx.h"
#include "Profiler.h"
#include "DateTime.h"
#include <fstream>
#include <format>
#include <filesystem>

void netlib::Profiler::Begin(const wstring_view name)
{
	Sample* sample = find_sample(name);

	if (sample == nullptr)
	{
		sample = get_empty_sample();
		sample->name = name;
		sample->callCount = 0;
	}
	else
	{
		if (sample->startTime.QuadPart != NULLTIME)
		{
			__debugbreak();
		}

		//sample->callCount++;
	}

	::QueryPerformanceCounter(&sample->startTime);
}

void netlib::Profiler::End(wstring_view name, LARGE_INTEGER endTime)
{
	Sample* sample = find_sample(name);

	if (sample == nullptr)
	{
		__debugbreak();
	}

	if (sample->startTime.QuadPart == NULLTIME)
	{
		__debugbreak();
	}

	const uint64 processTime = endTime.QuadPart - sample->startTime.QuadPart;

	sample->callCount += 1;
	sample->totalTime += processTime;
	sample->startTime.QuadPart = NULLTIME;

	// 최대, 최소값 갱신
	for (uint64& i : sample->max)
	{
		if (processTime > i)
		{
			i = processTime;
			break;
		}
	}

	for (uint64& i : sample->min)
	{
		if (processTime < i)
		{
			i = processTime;
			break;
		}

		if (i == 0)
		{
			i = processTime;
			break;
		}
	}
}

void netlib::Profiler::DataOut(wstring_view fileName)
{
	filesystem::path p{ L"./profile" };

	filesystem::create_directories(p);

	wofstream fout(fileName.data());
	fout.imbue(std::locale("kor"));


	const int64 freq = get_performance_frequency().QuadPart;
	const double perMicroSecond = static_cast<double>(freq) / static_cast<double>(SECOND_PER_MICRO_SECOND);

	for (auto it : _sampleMap)
	{
		fout << L"Thread ID : " << it.first << "\n";
		Sample* samples = it.second;
		fout << L"┌----------------------------------┬-------------------┬-------------------┬-------------------┬---------------┐\n";
		fout << L"│               Name               │      Average      │        Min        │        Max        │     Call      │\n";
		fout << L"├----------------------------------┼-------------------┼-------------------┼-------------------┼---------------┤\n";
		for (int32 i = 0; i < 100; i++)
		{
			Sample& sample = samples[i];
			double timeAverage;
			if (sample.callCount == 0)
			{
				continue;
			}

			if (sample.callCount > 4)
			{
				timeAverage =
					static_cast<double>(sample.totalTime - (sample.max[0] + sample.max[1] + sample.min[0] + sample.min[1]))
					/ (sample.callCount - 4) / perMicroSecond;
			}
			else
			{
				timeAverage =
					static_cast<double>(sample.totalTime) / sample.callCount / perMicroSecond;
			}

			fout << ::format(
				L"│ {:<32} │ {:>14.1f} μs │ {:>14.1f} μs │ {:>14.1f} μs │ {:>13} │\n",
				sample.name,
				timeAverage,
				sample.min[0] / perMicroSecond,
				sample.max[0] / perMicroSecond,
				sample.callCount
			);
		}
		fout << L"└----------------------------------┴-------------------┴-------------------┴-------------------┴---------------┘\n";
		fout << L"\n\n\n";


	}

	DateTime localTime;

	fout << ::format(
		L"Updated at {}\n",
		localTime.to_string()
	);

	fout.close();
}

void netlib::Profiler::DataOut()
{
	DateTime localTime;

	wstring fileName = ::format(L"profile/Profile_{}.txt",
		localTime.to_string(L"%Y%m%d_%H%M%S")
	);

	DataOut(fileName);
}

void netlib::Profiler::Reset()
{
	::memset(_profileSamples, 0, sizeof(_profileSamples));
}

void netlib::Profiler::Attach()
{
	WRITE_LOCK(_sampleMapLock);
	_sampleMap.insert(make_pair(GetCurrentThreadId(), _profileSamples));
}

LARGE_INTEGER netlib::Profiler::get_performance_frequency()
{
	static LARGE_INTEGER frequency{ 0, 0 };

	if (frequency.QuadPart == 0)
	{
		::QueryPerformanceFrequency(&frequency);
	}

	return frequency;
}

netlib::Profiler::Sample* netlib::Profiler::find_sample(wstring_view name)
{
	if (_isConstruct == false)
	{
		Attach();
		_isConstruct = true;
	}

	int idx = 0;

	while (_profileSamples[idx].useFlag != false)
	{
		if (_profileSamples[idx].name.compare(name) == 0)
		{
			return &_profileSamples[idx];
		}

		idx++;
	}

	return nullptr;
}

netlib::Profiler::Sample* netlib::Profiler::get_empty_sample()
{
	Sample* space = _profileSamples;
	while (space->useFlag != false)
	{
		space++;
	}

	space->useFlag = true;

	return space;
}
