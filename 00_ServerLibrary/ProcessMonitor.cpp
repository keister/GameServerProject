#include "stdafx.h"
#include "ProcessMonitor.h"
#include "Psapi.h"

netlib::ProcessMonitor::ProcessMonitor(HANDLE process)
	:_hProcess(process)
{


	PdhOpenQuery(nullptr, NULL, &_query);
	

	if (_hProcess == INVALID_HANDLE_VALUE)
	{
		_hProcess = GetCurrentProcess();
	}


	/// 프로세스 이름 찾기
	wstring processName;
	processName.resize(256);
	DWORD ret;
	GetModuleFileName(NULL, (LPWSTR)processName.data(), 256);

	//int32 r = QueryFullProcessImageName(_hProcess, NULL, processName.data(), &ret);

	// if (r == 0)
	// {
	// 	int32 err = GetLastError();
	// }
	int32 lastEscape = processName.rfind(L"\\") + 1;
	int32 dot = processName.rfind(L".exe");

	processName = processName.substr(lastEscape, dot - lastEscape);

	wstring queryStrNonPagedPoolUsage = L"\\Process(" + processName + L")\\Pool Nonpaged Byte";
	wstring queryStrPrivateMemoryUsage = L"\\Process(" + processName + L")\\Private Bytes";
	PdhAddCounter(_query, queryStrNonPagedPoolUsage.c_str(), NULL, &_nonPagedPoolUsageCounter);
	PdhAddCounter(_query, queryStrPrivateMemoryUsage.c_str(), NULL, &_privateMemoryUsageCounter);

	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	_numProcessors = systemInfo.dwNumberOfProcessors;

	Update();
}

netlib::ProcessMonitor::~ProcessMonitor()
{
	CloseHandle(_query);
	CloseHandle(_nonPagedPoolUsageCounter);
	CloseHandle(_privateMemoryUsageCounter);
}

void netlib::ProcessMonitor::Update()
{
	PdhCollectQueryData(_query);
	PDH_FMT_COUNTERVALUE nonPagedPoolUsage;
	PDH_FMT_COUNTERVALUE privateMemotyUsage;
	PdhGetFormattedCounterValue(_nonPagedPoolUsageCounter, PDH_FMT_LONG, NULL, &nonPagedPoolUsage);
	PdhGetFormattedCounterValue(_privateMemoryUsageCounter, PDH_FMT_LONG, NULL, &privateMemotyUsage);
	_nonPagedPoolUsage = nonPagedPoolUsage.longValue;
	_privateMemoryUsage = privateMemotyUsage.longValue;

	ULARGE_INTEGER none;
	ULARGE_INTEGER now;
	ULARGE_INTEGER kernel;
	ULARGE_INTEGER user;

	GetSystemTimeAsFileTime((LPFILETIME)&now);
	GetProcessTimes(_hProcess, (LPFILETIME)&none, (LPFILETIME)&none, (LPFILETIME)&kernel, (LPFILETIME)&user);

	ULONGLONG timeDiff = now.QuadPart - _prevTime.QuadPart;
	ULONGLONG kernelDiff = kernel.QuadPart - _prevKernelCpuUsage.QuadPart;
	ULONGLONG userDiff = user.QuadPart - _prevUserCpuUsage.QuadPart;

	ULONGLONG totalCpuUsage = kernelDiff + userDiff;

	_totalCpuUsage = (float64)totalCpuUsage / _numProcessors /  timeDiff * 100.0f;
	_userCpuUsage = (float64)(userDiff) / _numProcessors / timeDiff * 100.0f;
	_kernelCpuUsage = (float64)kernelDiff / _numProcessors / timeDiff * 100.0f;

	_prevKernelCpuUsage = kernel;
	_prevUserCpuUsage = user;
	_prevTime = now;
}
