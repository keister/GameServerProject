#pragma once
#include "Pdh.h"


namespace netlib
{
	class HardwareMonitor
	{
		enum
		{
			ETHERNET_MAX = 8,
		};

		struct EthernetMonitor
		{
			bool use;
			WCHAR name[128];

			PDH_HCOUNTER sendBytesCounter;
			PDH_HCOUNTER recvBytesCounter;
		};

	public:
		HardwareMonitor();
		~HardwareMonitor();

		void Update();

		uint32	GetNonPagedPoolUsage() { return _nonPagedPoolUsage; }
		uint32	GetAvailableMemoryUsage() { return _availableMemoryUsage; }
		float64 GetTotalCpuUsage() { return _totalCpuUsage; }
		float64 GetKernelCpuUsage() { return _kernelCpuUsage; }
		float64 GetUserCpuUsage() { return _userCpuUsage; }
		float64	GetNetworkSendBytes() { return _networkSendBytes; }
		float64	GetNetworkRecvBytes() { return _networkRecvBytes; }


	private:
		EthernetMonitor _ethernetMonitor[ETHERNET_MAX];

		PDH_HQUERY		_query;
		PDH_HCOUNTER	_nonPagedPoolUsageCounter;
		PDH_HCOUNTER	_availableMemoryUsageCounter;
		uint32			_nonPagedPoolUsage = 0;
		uint32			_availableMemoryUsage = 0;
		float64			_networkSendBytes = 0;
		float64			_networkRecvBytes = 0;


		// ÀüÃ¼ CPUÀÌ¿ë·ü
		float64			_totalCpuUsage = 0;
		float64         _kernelCpuUsage = 0;
		float64			_userCpuUsage = 0;

		ULARGE_INTEGER	_prevKernelCpuUsage{};
		ULARGE_INTEGER	_prevUserCpuUsage{};
		ULARGE_INTEGER	_prevIdleCpuUsage{};
	};

}
