#pragma once
#include "mysqlx/xdevapi.h"

using SqlSession = mysqlx::Session;

extern SqlSession& GetSqlSession();