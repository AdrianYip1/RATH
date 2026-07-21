#include "application.hpp"

#include "Rath/Renderer/context.hpp"
#include "Rath/Renderer/device.hpp"
#include "Rath/Renderer/renderer.hpp"

// Application Constructor
Rath::Application::Application(u32 width, u32 height, 
	const char* title) : window(width, height, title) {

	context = new Context();
}

// Application Destructor
Rath::Application::~Application() { 
	delete context;
}

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