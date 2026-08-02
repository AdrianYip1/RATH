#include "window.hpp"
#include <GLFW/glfw3.h>

// std
#include <stdexcept>
#include <cstdint>
#include <iostream>

// Window Constructor
Rath::Window::Window(u32 width, u32 height, const char* title) :
	WIDTH(width), HEIGHT(height), TITLE(title) {
	initWindow();
	std::cout << "Created window: " << TITLE << std::endl;
}

// Window Destructor
Rath::Window::~Window() {
	glfwDestroyWindow(window);
	std::cout << "Destroyed window: " << TITLE << std::endl;
	glfwTerminate();
}

// Return the current window
GLFWwindow* Rath::Window::getWindow() {
	return window;
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

	glfwSetWindowUserPointer(window, this);
	// Call framebufferResizeCallback when the window is resized
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

// Checks if the created window should be closed
bool Rath::Window::shouldClose() const {
	return glfwWindowShouldClose(window);
}

// Poll events such as closing window, resize, ...
void Rath::Window::pollEvents() {
	glfwPollEvents();
}

// Change framebufferResized
void Rath::Window::setFramebufferResized(bool info) {
	framebufferResized = info;
}

// Sets framebufferResized to true when a resize is detected
void Rath::Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}