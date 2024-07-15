#include "stdafx.h"

#include <ServerConfig.h>

#include "CrashDump.h"
#include "GameServer.h"

CrashDump cd;

int main()
{
	ServerConfig::AddPath(L"../ServerConfig.ini");

	SET_LOG_LEVEL(LogLevel::DBG);

	GameServer server(L"GameServer");

	server.Start();

	while (true)
	{
		
	}
	return 0;
}
