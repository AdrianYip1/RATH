#pragma once

// Rath
#include "Rath/Core/defines.hpp"
#include "enginemath/mat4.hpp"
#include "enginemath/vec3.hpp"
#include "enginemath/enginemath.hpp"

// std
#include <cmath>
#include <algorithm>

namespace Rath {
	class Camera {
		public:
			Camera(enginemath::Vec3 _position, f32 _aspect, f32 _yaw = enginemath::toRad(-90.0f),
				  f32 _pitch = 0.0f, f32 _fov = enginemath::toRad(45.0f), 
				  f32 _nearPlane = 0.1f, f32 _farPlane = 10.0f);
			
			enginemath::Vec3 getWorldUp() const;
			enginemath::Vec3 getForward() const;
			enginemath::Vec3 getRight() const;
			enginemath::Mat4 getView() const;
			enginemath::Mat4 getProj() const;

			// no roll for now
			void rotate(f32 dYaw, f32 dPitch);

			void move(enginemath::Vec3 delta);

			// Called on swapchain recreation
			void setAspect(f32 newAspect);

			void setDeltaTime(f32 _deltaTime);
			f32 getDeltaTime() { return deltaTime; };
			f32 getElapsedTime() { return elapsedTime; };
			

		private:
			enginemath::Vec3 worldUp = enginemath::Vec3(0.0f, 1.0f, 0.0f);

			enginemath::Vec3 position;
			f32 aspect;
			f32 yaw;
			f32 pitch;
			f32 fov;
			f32 nearPlane;
			f32 farPlane;

			f32 deltaTime;
			f32 elapsedTime;
	};
} // namespace Rath