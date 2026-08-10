#pragma once

// Rath
#include "Rath/Core/defines.hpp"
#include "Rath/Platform/input.hpp"
#include "camera.hpp"
#include "enginemath/mat4.hpp"
#include "Rath/Platform/window.hpp"

// std
#include <iostream>

struct GLFWwindow;

namespace Rath {
	class CameraController {
		public:
			CameraController(Window& _window, Input& _input, Camera& _camera);
			~CameraController();
			CameraController(const CameraController& other) = delete;
			CameraController& operator=(const CameraController& other) = delete;

			void checkCameraMovement();
			void checkMouse();
			void updateCamera();
			void changeCameraSpeed(f32 newSpeed);


		private:
			Window& window;
			Input& input;
			Camera& camera;
			f32 cameraSpeed = 20.0f;
			f64 previousX = 0.0f, previousY = 0.0f;
			f32 smoothX = 0.0f, smoothY = 0.0f;
			f64 xPos = 0.0f, yPos = 0.0f;
			f32 smoothFactor = 0.3f;
			// Accumulates the WASD movement per check 
			// to normalize and update camera at the end;
			enginemath::Vec3 movementCollector;

	};
} // namespace Rath