#pragma once

#include <memory>

#include "Rath/Core/defines.hpp"
#include "Rath/Platform/window.hpp"

namespace RATH{
	class Renderer; 

	class Application {
		public:
			Application(u32 width, u32 height, const char* title);
			~Application();
			Application& operator=(const Application& other) = delete;
			Application(Application&& other) = delete;

			// Entrypoint, runs the main loop
			void run();

		private:
			Window window;
			std::unique_ptr<Renderer> renderer;

			// Polls the window until it closes
			void mainLoop();
	};
} // namespace RATH
