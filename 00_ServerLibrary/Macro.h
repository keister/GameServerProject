#pragma once

#define CRASH(str) __debugbreak();
#define CRASH_LOG(type, fmt, ...) do\
	{\
	LOG_ERR(type, fmt, __VA_ARGS__);\
	__debugbreak();\
	}\
	while (false)
#define CRASH_ASSERT(expr) if ((expr) == false) __debugbreak();
#define CRASH_ASSERT_LOG(expr, type, fmt, ...) do \
	{ \
	if ((expr) == false) \
	{ \
		LOG_ERR(type, fmt, __VA_ARGS__); \
		__debugbreak(); \
	} \
} while (false)

#define CONCAT_MSG(a, b) a##b
#define CONCAT(a,b) CONCAT_MSG(a, b)
