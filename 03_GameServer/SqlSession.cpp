#include "stdafx.h"
#include "SqlSession.h"

#include "common/AppSettings.h"

namespace
{
	DWORD sqlSessionTlsIndex = TlsAlloc();
}

SqlSession& GetSqlSession()
{
	SqlSession* session = static_cast<SqlSession*>(TlsGetValue(sqlSessionTlsIndex));

	if (session == nullptr)
	{
		session = new SqlSession(AppSettings::GetSection("MySql")["uri"].get<string>());

		TlsSetValue(sqlSessionTlsIndex, session);
	}

	return *session;
}
