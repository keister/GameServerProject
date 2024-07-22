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
	uint64 prevFrame = 0;
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
		uint64 frame = server.GetFrameCount(Groups::TOWN);

		cout <<
			std::format(
				"\n\n\n\n\n\n"
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
				"             TOWN : {} / {}\n"
				"=======================================\n"
				"\n\n\n\n\n\n"
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
				, server.GetPlayerCount(Groups::TOWN)
				, frame - prevFrame
			);
		prevAcceptCount = acceptCount;
		prevSendCount = sendCount;
		prevRecvCount = recvCount;
		prevFrame = frame;

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
