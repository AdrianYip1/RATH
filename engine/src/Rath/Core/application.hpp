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

		void run();
		
	private:
		Window window;
		// TODO: use smart pointers instead?
		std::unique_ptr<Renderer> renderer;

		void mainLoop();

	};
} // namespace Rath