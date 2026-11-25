#pragma once

// Platform checks
#if defined(_WIN32) || defined(_WIN64)
	#define RATH_PLATFORM_WINDOWS 
#elif defined(__APPLE__)
	#define RATH_PLATFORM_APPLE 
	#error "platformDetection: apple not supported yet"
#elif defined(__ANDROID__)
	#define RATH_PLATFORM_ANDROID 
	#error "platformDetection: android not supported yet"
#elif defined(__linux__)
	#define RATH_PLATFORM_LINUX 
#else
	#error "platformDetection: unsupported platform"
#endif

// Debug for validation layers
#ifdef NDEBUG
	#define RATH_RELEASE 
#else
	#define RATH_DEBUG
#endif
