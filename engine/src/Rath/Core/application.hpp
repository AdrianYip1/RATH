#pragma once

// Rath files
#include "Rath/Platform/window.hpp"

// std
#include <memory>

namespace Rath {
	class Renderer;

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
		
		// Smmart pointer used for renderer to automatically manage
		// memory
		std::unique_ptr<Renderer> renderer;

		// Main loop which polls for window resize/close/minimize and starts the drawing loop
		void mainLoop();

	};
} // namespace Rath