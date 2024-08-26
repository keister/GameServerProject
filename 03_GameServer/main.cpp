#include "stdafx.h"

#include <ServerConfig.h>

#include "CrashDump.h"
#include "GameServer.h"
#include "Packet.h"
CrashDump cd;

int main()
{
	ServerConfig::AddPath(L"../ServerConfig.ini");

	SET_LOG_LEVEL(LogLevel::DBG);

	GameServer server(L"GameServer");
	uint64 prevAcceptCount = 0;
	uint64 prevSendCount = 0;
	uint64 prevRecvCount = 0;
	uint64 prevFrameca = 0;
	uint64 prevFrameCe = 0;
	uint64 prevPfCe = 0;
	uint64 prevPfca = 0;
	server.Start();

	DWORD idealTime = timeGetTime();
	DWORD overTime = 0;

	while (true)
	{
		if (_kbhit())
		{
			char ch = _getch();
			if (ch == 'Q' || ch == 'q')
			{
				break;
			}

		}

		uint64 acceptCount = server.GetAcceptCount();
		uint64 sendCount = server.GetSendMessageCount();
		uint64 recvCount = server.GetRecvMessageCount();
		uint64 frameCa = server.GetFrameCount(Groups::TOWN);
		uint64 frameCe = server.GetFrameCount(Groups::CEMETERY);
		uint64 pfCountCa = server.GetGroup(Groups::TOWN)->GetMap()->GetTotalPathFindingCount();
		uint64 pfCountCe = server.GetGroup(Groups::CEMETERY)->GetMap()->GetTotalPathFindingCount();
		cout <<
			std::format(
				"\n\n\n\n"
				"Q : QUIT | G : PROFILE SAVE\n"
				"=======================================\n"
				" Disconnect Total : {}\n"
				"     Accept Total : {}\n"
				"       Aceept TPS : {}\n"
				"         Send TPS : {}\n"
				"         Recv TPS : {}\n"
				"      Packet Pool : {} / {}\n"
				"=======================================\n"
				"          Session : {}\n"
				"=======================================\n"
				"            LOGIN : {}\n"
				"            LOBBY : {}\n"
				"=======================================\n"
				"             |  Canyon  | Cemetery |\n"
				"FPS          : {:<8} | {:<8} |\n"
				"Players      : {:<8} | {:<8} |\n"
				"Objects      : {:<8} | {:<8} |\n"
				"Path Finding : {:<8} | {:<8} |\n"
				"=======================================\n"
				"\n\n\n\n"
				, server.GetReleaseTotal()
				, acceptCount
				, acceptCount - prevAcceptCount
				, sendCount - prevSendCount
				, recvCount - prevRecvCount
				, Packet::GetUseCount()
				, Packet::GetCapacity()
				, server.GetSessionCount()
				, server.GetPlayerCount(Groups::LOGIN)
				, server.GetPlayerCount(Groups::LOBBY)
				, frameCa - prevFrameca
				, frameCe - prevFrameCe
				, server.GetPlayerCount(Groups::TOWN), server.GetPlayerCount(Groups::CEMETERY)
				, server.GetGroup(Groups::TOWN)->GetObjectCount(), server.GetGroup(Groups::CEMETERY)->GetObjectCount()
				, pfCountCa - prevPfca
				, pfCountCe - prevPfCe
			);
		prevAcceptCount = acceptCount;
		prevSendCount = sendCount;
		prevRecvCount = recvCount;
		prevFrameca = frameCa;
		prevPfca = pfCountCa;
		prevFrameCe = frameCe;
		prevPfCe = pfCountCe;
		::Sleep(1000 - overTime);
		idealTime += 1000;
		overTime = timeGetTime() - idealTime;
		if (overTime >= 1000)
		{
			overTime = 1000;
		}

	}
	return 0;
}
