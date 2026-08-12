#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "../images/texture.hpp"

#include "material.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <unordered_map>

namespace Rath {
	// The expanding model struct for RATH
	// The create info describes the mesh asset, no placements etc
	struct R_ModelCreateInfo {
		std::string modelPath;
		R_Material* material = nullptr;
	};

} // namespace Rath