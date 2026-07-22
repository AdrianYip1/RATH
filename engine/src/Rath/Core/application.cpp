#include "application.hpp"

#include "Rath/Renderer/renderer.hpp"

// Application Constructor
// 1. window creation
// 2. renderer creation (which creates the instance)
Rath::Application::Application(u32 width, u32 height, 
	const char* title) : window(width, height, title),
						 renderer(std::make_unique<Renderer>()) 
{

}

// Application Destructor
Rath::Application::~Application() { 

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