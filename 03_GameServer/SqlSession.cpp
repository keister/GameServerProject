#include "stdafx.h"
#include "SqlSession.h"

namespace
{
	DWORD sqlSessionTlsIndex = TlsAlloc();
}

SqlSession& GetSqlSession()
{
	SqlSession* session = static_cast<SqlSession*>(TlsGetValue(sqlSessionTlsIndex));

	if (session == nullptr)
	{
		session = new SqlSession("localhost", 33060, "root", "as1234", "game");
		TlsSetValue(sqlSessionTlsIndex, session);
	}

	return *session;
}
