#pragma once

// Rath files
#include "Rath/Platform/window.hpp"
#include "Rath/Platform/input.hpp"
#include "camera/camera.hpp"
#include "camera/cameraController.hpp"


// std
#include <chrono>
#include <memory>

namespace Rath {
	class Context;
	class Device;
	class Buffer;
	class Renderer;
	class UI;

	class Application {
	public:
		Application(u32 width, u32 height, const char* title);
		~Application();
		Application& operator=(const Application& other) = delete;
		Application(Application&& other) = delete;

		// Entrypoint to the application, runs mainLoop
		void run();

	private:
		Window window;
		Input input;
		Camera camera;
		CameraController cameraController;
		std::unique_ptr<Context> context;
		std::unique_ptr<Device> device;
		std::unique_ptr<Buffer> buffer;

		// Smart pointer used for renderer to automatically manage memory
		std::unique_ptr<Renderer> renderer;

		// Main loop which polls for window resize/close/minimize and starts the drawing loop
		void mainLoop();

	};
} // namespace Rath