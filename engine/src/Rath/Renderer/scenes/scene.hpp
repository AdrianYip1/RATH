#pragma once
#include <enginemath/mat4.hpp>
#include <enginemath/vec3.hpp>

// Rath files
#include "Rath/Core/defines.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <memory>


namespace Rath {
	class R_Model;

	struct R_SceneObject {
		// a pointer to a model
		R_Model* model = nullptr;
		// The default location of an object (starting point)
		enginemath::Mat4 baseTransform = enginemath::Mat4::identity();
		// Location of object
		enginemath::Mat4 transform = enginemath::Mat4::identity();
		// Colour of object
		enginemath::Vec3 color = enginemath::Vec3(1.0f);
	};

	struct R_SceneLight {
		// pointer to a model
		R_Model* model = nullptr;
		// Location of light
		enginemath::Vec3 position = enginemath::Vec3(0.0f);
		// Colour of light
		enginemath::Vec3 color = enginemath::Vec3(1.0f);
	};

	// Scene will hold all of the model information
	// Future: camera, lights
	struct R_Scene {
		std::vector<R_SceneObject> objects;
		std::vector<R_SceneLight> lights;
	};

} // namespace Rath