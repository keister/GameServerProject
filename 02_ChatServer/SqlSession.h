#pragma once
#include "mysqlx/xdevapi.h"

namespace chat
{
	using SqlSession = mysqlx::Session;

	extern SqlSession& GetSqlSession();
}
