#include "cameraController.hpp"
#include <GLFW/glfw3.h>

Rath::CameraController::CameraController(Input& _input, Camera& _camera) : 
	input(_input), camera(_camera) {

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