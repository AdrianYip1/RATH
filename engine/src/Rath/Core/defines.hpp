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
	const std::string MODEL_PATH = "../../../../engine/models/viking_room.obj";
	const std::string TEXTURE_PATH = "../../../../engine/textures/viking_room.png";

	const std::string MODEL2_PATH = "../../../../engine/models/donut2.obj";
	const std::string TEXTURE2_PATH = "../../../../engine/textures/viking_room.png";
	
	// The amount of frames that can be processed at the same time
	const int MAX_FRAMES_IN_FLIGHT = 2;

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

	// Placeholder for push constants -> drawing multiple objects 
	const u32 NUMBER_OF_ROOMS = 4;
} // namespace Rath
