#pragma once
#include "Rath/Platform/window.hpp"

namespace Rath {
	class Context;

	class Application {
	public:
		Application(u32 width, u32 height, const char* title);
		~Application();
		Application& operator=(const Application& other) = delete;
		Application(Application&& other) = delete;

		void run();
		
	private:
		Window window;
		Context* context;

		void mainLoop();

	};
} // namespace Rath