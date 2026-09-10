#pragma once

#include "Rath/Core/defines.hpp"
#include "Rath/Platform/window.hpp"

namespace Rath {
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

			// Polls the window until it closes
			void mainLoop();
	};
} // namespace Rath
