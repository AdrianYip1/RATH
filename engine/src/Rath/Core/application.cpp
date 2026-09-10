#include "application.hpp"

RATH::Application::Application(u32 width, u32 height, const char* title) :
	window(width, height, title) {
}

RATH::Application::~Application() {
}

void RATH::Application::run() {
	mainLoop();
}

void RATH::Application::mainLoop() {
	while (!window.shouldClose()) {
		window.pollEvents();
	}
}
