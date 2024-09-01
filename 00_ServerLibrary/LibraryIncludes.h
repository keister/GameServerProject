#pragma once

#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "Dbghelp.lib")
#pragma comment(lib, "Pdh.lib")

#include <iostream>



#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <timeapi.h>
#include <conio.h>
#include <process.h>
#include <string>
#include <algorithm>

#include "Types.h"
#include "Logger.h"
#include "Macro.h"
#include "Containers.h"
namespace netlib{}