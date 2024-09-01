#pragma once
#include <Pdh.h>

namespace netlib
{
	class ProcessMonitor
	{
	public:
		ProcessMonitor(HANDLE process = INVALID_HANDLE_VALUE);
		~ProcessMonitor();

		void Update();

		uint32	GetNonPagedPoolUsage() { return _nonPagedPoolUsage; }
		uint32	GetPrivateMemoryUsage() { return _privateMemoryUsage; }
		float64 GetTotalCpuUsage() { return _totalCpuUsage; }
		float64 GetKernelCpuUsage() { return _kernelCpuUsage; }
		float64 GetUserCpuUsage() { return _userCpuUsage; }


	private:
		PDH_HQUERY		_query;
		PDH_HCOUNTER	_nonPagedPoolUsageCounter;

		PDH_HCOUNTER	_privateMemoryUsageCounter;
		uint32			_nonPagedPoolUsage = 0;

		uint32			_privateMemoryUsage = 0;

		// 프로세스 CPU이용률
		HANDLE			_hProcess;

		int32			_numProcessors;
		float64			_totalCpuUsage = .0f;
		float64         _kernelCpuUsage = .0f;
		float64			_userCpuUsage = 0.0f;
		ULARGE_INTEGER	_prevKernelCpuUsage{};
		ULARGE_INTEGER	_prevUserCpuUsage{};
		ULARGE_INTEGER	_prevTime{};
	};
}

