#include <GLFW/glfw3.h>
#include "window.hpp"

RATH::Window::Window(u32 w, u32 h, const char* windowName) :
	WIDTH(w), HEIGHT(h), TITLE(windowName)
{
	initWindow();
}

RATH::Window::~Window() {
	glfwDestroyWindow(window);
	glfwTerminate();
}

void RATH::Window::initWindow() {
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	
	window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, nullptr, nullptr);

	if (!window) {
		throw std::runtime_error("ERROR: window.cpp -> initWindow()");
	}

	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
}

bool RATH::Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

void RATH::Window::pollEvents() {
	glfwPollEvents();
}

void RATH::Window::setFramebufferResized(bool info) {
	framebufferResized = info;
}

void RATH::Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
	app->framebufferResized = true;
}
