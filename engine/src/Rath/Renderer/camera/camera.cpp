#include "camera.hpp"

Rath::Camera::Camera(enginemath::Vec3 _position, f32 _aspect, f32 _yaw = enginemath::toRad(-90.0f),
					 f32 _pitch = 0.0f, f32 _fov = enginemath::toRad(45.0f),
					 f32 _near = 0.1f, f32 _far = 10.0f) : 
	position(_position), aspect(_aspect), yaw(_yaw), pitch(_pitch), fov(_fov),
	near(_near), far(_far) {

}

Rath::Camera::~Camera() {

}

enginemath::Vec3 Rath::Camera::getWorldUp() const {
	return worldUp;
}

// Returns the forward vector of the camera
// Convention is Y on the top, Z into page, X to the right
enginemath::Vec3 Rath::Camera::getForward() const {
	enginemath::Vec3 forward;
	forward.x = std::cos(pitch) * std::cos(yaw);
	forward.y = std::sin(pitch);
	forward.z = std::sin(yaw) * std::cos(pitch);

	return forward;
}

enginemath::Vec3 Rath::Camera::getRight() const {
	return getForward().cross(getWorldUp()).normalized();
}

enginemath::Mat4 Rath::Camera::getView() const {

}

enginemath::Mat4 Rath::Camera::getProj() const {

}

// Called on swapchain recreation
void Rath::Camera::setAspect(f32 newAspect) {

}