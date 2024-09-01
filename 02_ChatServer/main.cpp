#include "stdafx.h"
#include "CrashDump.h"

#include "ChatServer.h" 
#include "Profiler.h"
#include "Packet.h"
#include "common/AppSettings.h"
CrashDump cd;

using namespace chat;

int main()
{

	SET_LOG_LEVEL(LogLevel::DBG);

	ChatServer server(AppSettings::GetSection("ChatServer"));

	uint64 prevAuthCount = 0;
	uint64 prevAcceptCount = 0;
	uint64 prevSendCount = 0;
	uint64 prevRecvCount = 0;

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

			if (ch == 'G' || ch == 'g')
			{
				PROFILE_DATA_OUT();
			}
		}

		uint64 acceptCount = server.GetAcceptCount();
		uint64 sendCount = server.GetSendMessageCount();
		uint64 recvCount = server.GetRecvMessageCount();

		cout <<
			std::format(
				"\n\n\n\n\n\n\n"
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
				"\n\n\n\n\n\n\n"
				, server.GetReleaseTotal()
				, acceptCount
				, acceptCount - prevAcceptCount
				, sendCount - prevSendCount
				, recvCount - prevRecvCount
				, Packet::GetUseCount()
				, Packet::GetCapacity()
				, server.GetSessionCount()
			);

		prevAcceptCount = acceptCount;
		prevSendCount = sendCount;
		prevRecvCount = recvCount;

		::Sleep(1000 - overTime);
		idealTime += 1000;
		overTime = timeGetTime() - idealTime;
		if (overTime >= 1000)
		{
			overTime = 1000;
		}

	}

	server.Stop();
	return 0;
}
