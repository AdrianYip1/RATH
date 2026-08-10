#include "application.hpp"

// Rath files
#include "Rath/Renderer/renderer.hpp"

// Application Constructor
Rath::Application::Application(u32 width, u32 height, 
	const char* title) : 
	window(width, height, title),
	input(window), camera(enginemath::Vec3(0.0f, 0.5f, 4.0f), width / (f32)height), 
	cameraController(window, input, camera),
	renderer(std::make_unique<Renderer>(window, camera)) 
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

// Main loop which polls for window resize/close/minimize
// and starts the drawing loop
// Has delta time which gets threaded down to uniform
void Rath::Application::mainLoop() {
	// Init the clock + last time point
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!window.shouldClose()) {
		// Delta time
		auto currentTime = std::chrono::high_resolution_clock::now();

		f32 deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;
		camera.setDeltaTime(deltaTime);

		window.pollEvents();
		// Accumulate controls before updating camera
		cameraController.checkCameraMovement();
		cameraController.checkMouse();
		cameraController.updateCamera();
		renderer->drawFrame();
	}
	renderer->wait();
} 