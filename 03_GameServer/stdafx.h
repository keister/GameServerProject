#pragma once

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define NOMINMAX
#include "LibraryIncludes.h"
//#include "../lib/hiredis/CRedisConn.h"
#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "Libs/cpp-httplib/httplib.h"
#include "Libs/Json/json.hpp"
#include <Dense>
using json = nlohmann::json;
using Position = Eigen::Vector2<float32>;
using namespace std;
using namespace netlib;

//#define PROFILE
