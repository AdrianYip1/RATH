#pragma once

// Rath
#include "Rath/Core/defines.hpp"
#include "Rath/Platform/input.hpp"
#include "camera.hpp"
#include "enginemath/mat4.hpp"

// std
#include <iostream>

namespace Rath {
	class CameraController {
		public:
			CameraController(Input& _input, Camera& _camera);
			~CameraController();
			CameraController(const CameraController& other) = delete;
			CameraController& operator=(const CameraController& other) = delete;

			void checkCameraMovement();
			void updateCamera();
			void changeCameraSpeed(f32 newSpeed);


		private:
			Input& input;
			Camera& camera;
			f32 cameraSpeed = 20.0f;

			// Accumulates the WASD movement per check 
			// to normalize and update camera at the end;
			enginemath::Vec3 movementCollector;

	};
} // namespace Rath