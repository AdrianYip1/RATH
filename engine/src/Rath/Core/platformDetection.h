#pragma once

#if defined(_WIN32)
	#define RATH_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
	#define RATH_PLATFORM_APPLE 1
	#error "platformDetection: apple not supported yet"
#elif defined(__ANDROID__)
	#define RATH_PLATFORM_ANDROID 1
	#error "platformDetection: android not supported yet"
#elif defined(__linux__)
	#define RATH_PLATFORM_LINUX 1
#else
	#error "platformDetection: unsupported platform"
#endif