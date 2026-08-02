#include "Rath/Core/defines.hpp"
#include "Rath/Core/application.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
// Detects the platform RATH is being ran on
#if defined(RATH_PLATFORM_WINDOWS)
	const char* platform = "Windows";
#elif defined(RATH_PLATFORM_LINUX)
	const char* platform = "Linux";
#else
	const char* platform = "unknown";
#endif
	
	// Init application with dimensions and name
	Rath::Application app(1600, 1200, "RATH");
	try {
		// Start render loop
		app.run();
	}
	// Catches any exceptions and runtime errors
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;

}
