#include "input.hpp"
#include <GLFW/glfw3.h>

// std
#include <stdexcept>
#include <cstdint>
#include <iostream>

Rath::Input::Input(Window& _window) : window(_window) {

}

Rath::Input::~Input() {

}


bool Rath::Input::isKeyPressed(int key) {
	return glfwGetKey(window.getWindow(), key) == GLFW_PRESS;
}

bool Rath::Input::isMousePressed(int button) {
	return glfwGetMouseButton(window.getWindow(), button) == GLFW_PRESS;
}
