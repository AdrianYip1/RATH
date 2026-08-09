#include "camera.hpp"

Rath::Camera::Camera(enginemath::Vec3 _position, f32 _aspect, f32 _yaw,
					 f32 _pitch, f32 _fov,
					 f32 _near, f32 _far) : 
	position(_position), aspect(_aspect), yaw(_yaw), pitch(_pitch), fov(_fov),
	nearPlane(_near), farPlane(_far) {
	deltaTime = 0.0f;
	elapsedTime = 0.0f;
}


enginemath::Vec3 Rath::Camera::getWorldUp() const {
	return worldUp;
}

// Returns the forward vector of the camera
// Convention is Y on the top, -Z into page, X to the right
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
	// cameraPos -> position
	// targetPot -> position + getForward() since it gets the camera's position
	// and figures out the direction its facing
	// upvec -> getWorldUp() for global up position
	return enginemath::Mat4::lookAtM(position, position + getForward(), getWorldUp());

}

enginemath::Mat4 Rath::Camera::getProj() const {
	enginemath::Mat4 proj = enginemath::Mat4::projectionM(fov, aspect, nearPlane, farPlane);
	// Vulkan's Y coord in clip space is inverted compared to OpenGL's
	proj.m[1][1] *= -1;
	return proj;
}

void Rath::Camera::rotate(f32 dYaw, f32 dPitch) {
	yaw += dYaw;
	// To wrap around the yaw (left/right turning) so it doesn't get super large
	// while keeping the same values
	if (yaw >= 2 * enginemath::PI) {
		yaw -= 2 * enginemath::PI;
	}
	if (yaw <= -2 * enginemath::PI) {
		yaw += 2 * enginemath::PI;
	}

	// Clamp pitch (looking up and down) 
	pitch = std::clamp(pitch + dPitch, enginemath::toRad(-89.0f), enginemath::toRad(89.0f));
}

void Rath::Camera::move(enginemath::Vec3 delta) {
	position += delta;
}

// Called on swapchain recreation
void Rath::Camera::setAspect(f32 newAspect) {
	aspect = newAspect;
}

void Rath::Camera::setDeltaTime(f32 _deltaTime) {
	deltaTime = _deltaTime;
	elapsedTime += _deltaTime;
}
