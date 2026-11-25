#include "visualizer.hpp"

#include <cstdlib>

int main() {
	
	RATH::ApplicationConfig config = {
		.title = "RATH",
		.width = 1280,
		.height = 720
	};

	try {
		Visualizer app(config);
		app.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
