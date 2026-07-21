#include "application.hpp"

// Application Constructor
Rath::Application::Application(u32 width, u32 height, 
	const char* title) : window(width, height, title) {
}

// Application Destructor
Rath::Application::~Application() { }

// Entrypoint to the application -> calls initVulkan()
// then runs mainLoop()
void Rath::Application::run() {
	// initVulkan();
	mainLoop();
}

// Main loop
void Rath::Application::mainLoop() {
	while (!window.shouldClose()) {
		window.pollEvents();
	}
} 