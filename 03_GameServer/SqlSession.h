#pragma once
#include "mysqlx/xdevapi.h"

namespace game
{
	using SqlSession = mysqlx::Session;

	extern SqlSession& GetSqlSession();
}