#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.

#include "LibraryIncludes.h"
//#include "../lib/hiredis/CRedisConn.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "Libs/cpp-httplib/httplib.h"
#include "Libs/Json/json.hpp"
using json = nlohmann::json;
using namespace std;

//#define PROFILE
