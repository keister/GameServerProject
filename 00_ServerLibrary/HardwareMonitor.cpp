#include "stdafx.h"
#include "HardwareMonitor.h"

#include <strsafe.h>

netlib::HardwareMonitor::HardwareMonitor()
{
	PdhOpenQuery(nullptr, NULL, &_query);
	PdhAddCounter(_query, L"\\Memory\\Pool Nonpaged Bytes", NULL, &_nonPagedPoolUsageCounter);
	PdhAddCounter(_query, L"\\Memory\\Available MBytes", NULL, &_availableMemoryUsageCounter);

	int32 cnt = 0;
	WCHAR* cur = nullptr;
	WCHAR* counters = nullptr;
	WCHAR* interfaces = nullptr;
	DWORD counterSize = 0;
	DWORD interfaceSize = 0;

	PdhEnumObjectItems(nullptr, nullptr, L"Network Interface", counters, &counterSize, interfaces, &interfaceSize, PERF_DETAIL_WIZARD, 0);

	counters = new WCHAR[counterSize];
	interfaces = new WCHAR[interfaceSize];

	if (PdhEnumObjectItems(nullptr, nullptr, L"Network Interface", counters, &counterSize, interfaces, &interfaceSize, PERF_DETAIL_WIZARD, 0)
		!= ERROR_SUCCESS)
	{
		delete[] counters;
		delete[] interfaces;
	}

	cur = interfaces;
	WCHAR queryStr[1024] = { 0, };
	for (; *cur != L'\0' && cnt < ETHERNET_MAX; cur += wcslen(cur) + 1, cnt++)
	{
		EthernetMonitor& ether = _ethernetMonitor[cnt];
		ether.use = true;
		ether.name[0] = L'\0';

		wcscpy_s(ether.name, cur);
		queryStr[0] = L'\0';
		StringCbPrintf(queryStr, sizeof(WCHAR) * 1024, L"\\Network Interface(%s)\\Bytes Received/sec", cur);
		PdhAddCounter(_query, queryStr, NULL, &ether.recvBytesCounter);
		queryStr[0] = L'\0';
		StringCbPrintf(queryStr, sizeof(WCHAR) * 1024, L"\\Network Interface(%s)\\Bytes Sent/sec", cur);
		PdhAddCounter(_query, queryStr, NULL, &ether.sendBytesCounter);
	}

	Update();
}

netlib::HardwareMonitor::~HardwareMonitor()
{
	CloseHandle(_query);
	CloseHandle(_nonPagedPoolUsageCounter);
	CloseHandle(_availableMemoryUsageCounter);
}

void netlib::HardwareMonitor::Update()
{
	PdhCollectQueryData(_query);
	PDH_FMT_COUNTERVALUE nonPagedPoolUsage;
	PDH_FMT_COUNTERVALUE availableMemoryUsage;
	PdhGetFormattedCounterValue(_nonPagedPoolUsageCounter, PDH_FMT_LONG, NULL, &nonPagedPoolUsage);
	PdhGetFormattedCounterValue(_availableMemoryUsageCounter, PDH_FMT_LONG, NULL, &availableMemoryUsage);
	_nonPagedPoolUsage = nonPagedPoolUsage.longValue;
	_availableMemoryUsage = availableMemoryUsage.longValue;


	_networkSendBytes = 0;
	_networkRecvBytes = 0;
	for (int32 cnt = 0; cnt < ETHERNET_MAX; cnt++)
	{
		PDH_FMT_COUNTERVALUE counterValue;
		if (_ethernetMonitor[cnt].use == true)
		{
			PdhGetFormattedCounterValue(_ethernetMonitor[cnt].recvBytesCounter,
				PDH_FMT_DOUBLE, NULL, &counterValue);
			_networkRecvBytes += counterValue.doubleValue;

			PdhGetFormattedCounterValue(_ethernetMonitor[cnt].sendBytesCounter,
				PDH_FMT_DOUBLE, NULL, &counterValue);
			_networkSendBytes += counterValue.doubleValue;
		}
	}

	ULARGE_INTEGER idle;
	ULARGE_INTEGER kernel;
	ULARGE_INTEGER user;

	if (GetSystemTimes((PFILETIME)&idle, (PFILETIME)&kernel, (PFILETIME)&user) == false)
	{
		return;
	}

	ULONGLONG kernelDiff = kernel.QuadPart - _prevKernelCpuUsage.QuadPart;
	ULONGLONG userDiff = user.QuadPart - _prevUserCpuUsage.QuadPart;
	ULONGLONG idleDiff = idle.QuadPart - _prevIdleCpuUsage.QuadPart;

	ULONGLONG totalCpuUsage = kernelDiff + userDiff;

	if (totalCpuUsage == 0)
	{
		_totalCpuUsage = 0;
		_userCpuUsage = 0;
		_kernelCpuUsage = 0;
	}
	else
	{
		_totalCpuUsage = (float64)(totalCpuUsage - idleDiff) / totalCpuUsage * 100.0f;
		_userCpuUsage = (float64)(userDiff) / totalCpuUsage * 100.0f;
		_kernelCpuUsage = (float64)(kernelDiff - idleDiff) / totalCpuUsage * 100.0f;
	}

	_prevKernelCpuUsage = kernel;
	_prevUserCpuUsage = user;
	_prevIdleCpuUsage = idle;
}
