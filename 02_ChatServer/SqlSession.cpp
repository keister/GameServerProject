#include "stdafx.h"
#include "SqlSession.h"

#include "common/AppSettings.h"

namespace chat
{
	namespace
	{
		DWORD sqlSessionTlsIndex = TlsAlloc();
	}

	SqlSession& GetSqlSession()
	{
		SqlSession* session = static_cast<SqlSession*>(TlsGetValue(sqlSessionTlsIndex));

		if (session == nullptr)
		{
			//session = new SqlSession("localhost", 11771, "root", "as1234", "game");
			session = new SqlSession(AppSettings::GetSection("MySql")["uri"].get<string>());
			TlsSetValue(sqlSessionTlsIndex, session);
		}

		return *session;
	}
	
}
