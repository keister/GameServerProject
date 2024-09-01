#pragma once

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
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
