#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/mat4.hpp>
#include <enginemath/vec3.hpp>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "../models/model.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <memory>


namespace Rath {
	struct R_SceneObject {
		// a pointer to a model
		R_Model* model = nullptr;
		// Location of object
		enginemath::Mat4 transform;
		// Colour of object
		enginemath::Vec3 colors;
	};

	// Scene will hold all of the model information
	// Future: camera, lights
	struct R_Scene {
		std::vector<R_SceneObject> objects;
	};

	class Scene {
		public:

		private:
			R_Scene scenee;
	};
} // namespace Rath