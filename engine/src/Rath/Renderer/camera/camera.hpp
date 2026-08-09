#pragma once

// Rath
#include "Rath/Core/defines.hpp"
#include "enginemath/mat4.hpp"
#include "enginemath/vec3.hpp"
#include "enginemath/enginemath.hpp"

// std
#include <cmath>

namespace Rath {
	class Camera {
		public:
			Camera(enginemath::Vec3 _position, f32 aspect, f32 yaw = enginemath::toRad(-90.0),
				  f32 pitch = 0.0f, f32 fov = enginemath::toRad(45.0f), 
				  f32 near = 0.1f, f32 far = 10.0f);
			~Camera();

			enginemath::Vec3 getWorldUp() const;
			enginemath::Vec3 getForward() const;
			enginemath::Vec3 getRight() const;
			enginemath::Mat4 getView() const;
			enginemath::Mat4 getProj() const;

			// Called on swapchain recreation
			void setAspect(f32 newAspect);

		private:
			enginemath::Vec3 worldUp = enginemath::Vec3(0.0f, 1.0f, 0.0f);

			enginemath::Vec3 position;
			f32 aspect;
			f32 yaw;
			f32 pitch;
			f32 fov;
			f32 near;
			f32 far;
	};
} // namespace Rath