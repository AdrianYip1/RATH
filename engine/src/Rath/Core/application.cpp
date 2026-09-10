#include "application.hpp"

#include "Rath/VK/renderer.hpp"   

RATH::Application::Application(u32 width, u32 height, const char* title) :
	window(width, height, title),
	renderer(std::make_unique<Renderer>(window)) {
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
