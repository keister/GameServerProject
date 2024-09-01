#pragma once
#include "mysqlx/xdevapi.h"

namespace login
{
	using SqlSession = mysqlx::Session;

	extern SqlSession& GetSqlSession();
}