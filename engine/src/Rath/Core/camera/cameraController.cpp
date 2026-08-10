#include "cameraController.hpp"
#include <GLFW/glfw3.h>

Rath::CameraController::CameraController(Window& _window, Input& _input, Camera& _camera) :
	input(_input), camera(_camera), window(_window) {
	glfwSetInputMode(window.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
	glfwGetCursorPos(window.getWindow(), &previousX, &previousY);
}

Rath::CameraController::~CameraController() {

}

void Rath::CameraController::checkCameraMovement() {
	movementCollector = enginemath::Vec3(0.0f, 0.0f, 0.0f);
	if (input.isKeyPressed(GLFW_KEY_W)) {
		movementCollector += camera.getForward();
	}

	if (input.isKeyPressed(GLFW_KEY_A)) {
		movementCollector -= camera.getRight();
	}

	if (input.isKeyPressed(GLFW_KEY_S)) {
		movementCollector -= camera.getForward();
	}

	if (input.isKeyPressed(GLFW_KEY_D)) {
		movementCollector += camera.getRight();
	}

	// Guard against the enginemath assert for normalize
	// Skips normalizing if movementCollector is 0
	if (!movementCollector.basicallyZero()) {
		movementCollector.normalize();
	}
}

void Rath::CameraController::checkMouse() {
	glfwGetCursorPos(window.getWindow(), &xPos, &yPos);

	f32 dX = static_cast<f32>(xPos - previousX);
	f32 dY = static_cast<f32>(yPos - previousY);

	// Lerp to smooth cursor
	// Gives the camera a fraction of the mouse movement every frame where the
	// rest carry over into the next frame.
	// Example: a flick from 2 corners is gradually given to the camera rotate instead of all
	// at once, giving a smoother effect
	smoothX = smoothX + (dX - smoothX) * smoothFactor;
	smoothY = smoothY + (dY - smoothY) * smoothFactor;

	camera.rotate(smoothX, smoothY);

	previousX = xPos;
	previousY = yPos;
}

void Rath::CameraController::updateCamera() {
	if (movementCollector != enginemath::Vec3(0.0f, 0.0f, 0.0f)) {
		camera.move(movementCollector * camera.getDeltaTime() * cameraSpeed);
	}
}

void Rath::CameraController::changeCameraSpeed(f32 newSpeed) {
	if (newSpeed > 0) {
		cameraSpeed = newSpeed;
		return;
	}
	std::cout << "Camera speed must be greater than 0.0f" << std::endl;
	return;
}