#pragma once

#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

struct GLFWwindow;

namespace RATH {
	class Window {
	public:
		Window(u32 w, u32 h, const char* windowName);
		~Window();
		Window(const Window& other) = delete;
		Window& operator=(const Window& other) = delete;

		GLFWwindow* getWindow() { return window; };

		bool shouldClose();

		void pollEvents();

		// Changes framebufferResized
		void setFramebufferResized(bool info);
		bool getFramebufferResized() { return framebufferResized; };

	private:
		GLFWwindow* window = nullptr;
		u32 WIDTH;
		u32 HEIGHT;
		const char* TITLE;
		bool framebufferResized = false;

		void initWindow();

		// Sets framebufferResized to true when a resize is detected
		static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	};

} // namespace RATH