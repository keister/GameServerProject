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
		session = new SqlSession("procademyserver.iptime.org", 11771, "remote", "!Khj378400", "game");
		TlsSetValue(sqlSessionTlsIndex, session);
	}

	return *session;
}
