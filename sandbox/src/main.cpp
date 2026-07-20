#include "Rath/Core/platformDetection.h"

#include <iostream>
#include <enginemath/vec3.hpp>

int main() {
#if defined(RATH_PLATFORM_WINDOWS)
	const char* platform = "Windows";
#elif defined(RATH_PLATFORM_LINUX)
	const char* platform = "Linux";
#else
	const char* platform = "unknown";
#endif

	enginemath::Vec3 v(1.0f, 2.0f, 3.0f);

	std::cout << "Rath sandbox up on " << platform << "\n";
	std::cout << "enginemath vec: " << v.x << ", " << v.y << ", " << v.z << "\n";
	return 0;
}
