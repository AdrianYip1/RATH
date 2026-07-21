#include "application.hpp"

// Application Constructor
Rath::Application::Application(u32 width, u32 height, 
	const char* title) : window(width, height, title) {
}

Rath::Application::~Application() {

}

void Rath::Application::run() {
	mainLoop();
}

void Rath::Application::mainLoop() {
	while (!window.shouldClose()) {
		window.pollEvents();
	}
} 