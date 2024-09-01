#include "stdafx.h"
#include "Logger.h"
#include <fstream>
#include <strsafe.h>
#include "DateTime.h"

void netlib::Logger::Log(const WCHAR* type, LogLevel logLevel, const WCHAR* fmt, ...)
{
	if (logLevel < level) // 로그 레벨이 낮으면 로그를 남기지 않는다.
	{
		return;
	}


	filesystem::create_directories(_path);

	int32 counter = InterlockedIncrement((LONG*)&_logCounter) - 1;

	WCHAR logMsg[256];
	WCHAR fullText[300];

	va_list va;
	va_start(va, fmt);
	int32 result = StringCchVPrintf(logMsg, 256, fmt, va);
	va_end(va);

	DateTime date;

	StringCchPrintf(fullText, 300,
		L"[%s] [%s / %08d] %s\n",
		_typeStrings[(int32)logLevel].c_str(),
		date.to_string().c_str(),
		counter,
		logMsg
	);

	WCHAR fileName[40];

	StringCchPrintf(fileName, 40,
		L"%s/%s_%s.txt",
		_path.c_str(),
		date.to_string(L"%Y%m"),
		type
	);

	wofstream fout{ fileName, ios_base::app};

	fout << fullText;

	fout.close();
}