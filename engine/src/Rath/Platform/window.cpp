#include "window.hpp"
#include <GLFW/glfw3.h>

// std
#include <Rath/Core/defines.hpp>

// Window Constructor
RATH::Window::Window(u32 width, u32 height, const char* title) :
	WIDTH(width), HEIGHT(height), TITLE(title) {
	initWindow();
}

// Window Destructor
RATH::Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Return the current window
GLFWwindow* RATH::Window::getWindow() {
	return window;
}

// Set up glfw window contexts and create the window
void RATH::Window::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(WIDTH, HEIGHT, 
							  TITLE, nullptr, nullptr);

	if (!window) {
		throw std::runtime_error("ERROR: window.cpp -> initWindow()");
	}

	glfwSetWindowUserPointer(window, this);
	// Call framebufferResizeCallback when the window is resized
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

// Checks if the created window should be closed
bool RATH::Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

// Poll events such as closing window, resize, ...
void RATH::Window::pollEvents() {
	glfwPollEvents();
}

// Change framebufferResized
void RATH::Window::setFramebufferResized(bool info) {
	framebufferResized = info;
}

// Sets framebufferResized to true when a resize is detected
void RATH::Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}