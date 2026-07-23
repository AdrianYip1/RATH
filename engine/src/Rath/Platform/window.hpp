#pragma once
#define GLFW_INCLUDE_VULKAN

// Rath files
#include "Rath/Core/defines.hpp"


struct GLFWwindow;

namespace Rath {
	class Window {
		public:
			Window(u32 width, u32 height, const char* title);
			~Window();
			Window(const Window& other) = delete;
			Window& operator=(const Window& other) = delete;

			GLFWwindow* getWindow();

			void initWindow();

			bool shouldClose() const;

			void pollEvents();
			
		private:
			GLFWwindow* window = nullptr;
			u32 WIDTH;
			u32 HEIGHT;
			const char* TITLE;
	};

} // namespace Rath