// Aliases, detection for platform/debug mode
#pragma once

// Rath files
#include "Rath/Core/platformDetection.hpp"
#include "Rath/Core/asserts.hpp"

// std
#include <cstdint>
#include <cstddef>
#include <string>

// Short aliases for commonly used types
namespace Rath {
	// Defining the model and texture paths (move later)
	const std::string MODEL_PATH = "../../../../engine/models/head.gltf";
	const std::string TEXTURE_PATH = "../../../../engine/textures/viking_room.png";

	const std::string MODEL2_PATH = "../../../../engine/models/head.gltf";
	const std::string TEXTURE2_PATH = "../../../../engine/textures/texture.jpg";
	
	// The amount of frames that can be processed at the same time
	const int MAX_FRAMES_IN_FLIGHT = 2;

	const int MAX_LIGHTS = 3;

	using size = size_t;

	using u8  = uint8_t;
	using u16 = uint16_t;
	using u32 = uint32_t;
	using u64 = uint64_t;

	using i8  = int8_t;
	using i16 = int16_t;
	using i32 = int32_t;
	using i64 = int64_t;

	using f32 = float;
	using f64 = double;

	// Specify the type of descriptor layout is being passed
	// into createDescriptorSetLayout()
	enum class R_DESCRIPTOR_TYPE {
		R_TYPE_UNIFORM,
		R_TYPE_SAMPLER
	};

	// Specifies the type of scene object (object/light) that will be spawned in
	// with pending spawn vector
	enum class R_SCENE_TYPE {
		R_SCENE_TYPE_OBJECT,
		R_SCENE_TYPE_LIGHT
	};

} // namespace Rath
