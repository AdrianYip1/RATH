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

			// Returns window
			GLFWwindow* getWindow();

			// Sets up the glfw window contexts and creates the window
			void initWindow();

			// Checks if the created window should be closed
			bool shouldClose() const;

			// Polls events such as closing the window, resize, ...
			void pollEvents();

			// Returns framebufferResized
			bool getFramebufferResized() { return framebufferResized; };

			// Changes framebufferResized
			void setFramebufferResized(bool info);
			
		private:
			GLFWwindow* window = nullptr;
			u32 WIDTH;
			u32 HEIGHT;
			const char* TITLE;
			bool framebufferResized = false;

			// Sets framebufferResized to true when a resize is detected
			static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	};
} // namespace Rath