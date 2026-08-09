#pragma once

// Rath
#include "Rath/Core/defines.hpp"
#include "Rath/Platform/input.hpp"
#include "enginemath/mat4.hpp"

namespace Rath {
	class CameraController {
		public:
			CameraController(Input& _input);
			~CameraController();
			CameraController(const CameraController& other) = delete;
			CameraController& operator=(const CameraController& other) = delete;

			void checkCameraMovement();

		private:
			Input& input;

			void updateCamera();
	};
} // namespace Rath