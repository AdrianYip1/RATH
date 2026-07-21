#include "window.hpp"
#include <GLFW/glfw3.h>
#include <stdexcept>

// Window Constructor
Rath::Window::Window(u32 width, u32 height, const char* title) :
	WIDTH(width), HEIGHT(height), TITLE(title) {
	initWindow();
}

// Window Destructor
Rath::Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Set up glfw window contexts and create the window
void Rath::Window::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, 
							  TITLE, nullptr, nullptr);

	if (!window) {
		throw std::runtime_error("Failed to create window");
	}
}

bool Rath::Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

void Rath::Window::pollEvents() {
	glfwPollEvents();
}