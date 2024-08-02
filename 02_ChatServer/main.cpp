#include "stdafx.h"
#include "CrashDump.h"

CrashDump cd;

int main()
{

	uint64 id = typeid(int).hash_code();

	return 0;
}
