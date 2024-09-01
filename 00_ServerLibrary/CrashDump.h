#pragma once
#include "CrashDump.h"
#include <format>
#include <minidumpapiset.h>

#include "DateTime.h"


namespace netlib
{

	class CrashDump
	{
	public:
		CrashDump()
		{
			_dumpCount = 0;
			_invalid_parameter_handler oldHandler;
			_invalid_parameter_handler newHandler;
			newHandler = MyInvalidParameterHandler;

			oldHandler = _set_invalid_parameter_handler(newHandler);
			_CrtSetReportMode(_CRT_WARN, 0);
			_CrtSetReportMode(_CRT_ASSERT, 0);
			_CrtSetReportMode(_CRT_ERROR, 0);

			_CrtSetReportHook(CustomReportHook);

			_set_purecall_handler(MyPurecallHandler);

			SetHandlerDump();
		}

		static LONG WINAPI MyExceptionFilter(PEXCEPTION_POINTERS exceptionPointer)
		{
			wcout << L"ExceptionFilter" << endl;
			long dumpCount = InterlockedIncrement(&_dumpCount);
			wcout << "interlocked" << endl;
			wstring filename;
			DateTime now{};
			wcout << "now" << endl;
			filename = ::format(L"Dump_{}_{}.dmp", now.to_string(L"%Y%m%d_%H%M%S"), dumpCount);


			wcout << filename << endl;

			HANDLE hDumpFile = ::CreateFile(
				filename.c_str(),
				GENERIC_WRITE,
				FILE_SHARE_WRITE,
				nullptr,
				CREATE_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr
			);

			wcout << L"FileWrite" << endl;

			HANDLE evDumpClear = CreateEvent(nullptr, true, false, nullptr);

			if (hDumpFile != INVALID_HANDLE_VALUE)
			{
				if (dumpCount == 1)
				{
					_MINIDUMP_EXCEPTION_INFORMATION minidumpExceptionInformation;

					minidumpExceptionInformation.ThreadId = ::GetCurrentThreadId();
					minidumpExceptionInformation.ExceptionPointers = exceptionPointer;
					minidumpExceptionInformation.ClientPointers = TRUE;

					MiniDumpWriteDump(
						GetCurrentProcess(),
						GetCurrentProcessId(),
						hDumpFile,
						MiniDumpWithFullMemory,
						&minidumpExceptionInformation,
						nullptr,
						nullptr
					);

					CloseHandle(hDumpFile);
					wcout << L"Creash Dump Save Finish!";
					SetEvent(evDumpClear);
				}

				WaitForSingleObject(evDumpClear, INFINITE);
			}

			return EXCEPTION_EXECUTE_HANDLER;
		}

		static void SetHandlerDump()
		{
			SetUnhandledExceptionFilter(MyExceptionFilter);
		}
		static void MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int	line, uintptr_t	reserved)
		{
			CRASH("InvalidParameter");
		}
		static void MyPurecallHandler()
		{
			CRASH("Purecall Error");
		}
		static int	CustomReportHook(int ireposttype, char* message, int* returnvalue)
		{
			CRASH("Error");
			return 0;
		}

		inline static long _dumpCount;
	};



}
