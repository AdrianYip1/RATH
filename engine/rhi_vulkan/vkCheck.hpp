#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#define VK_CHECK(x) do { \
	VkResult err = (x); \
	if (err != VK_SUCCESS) { \
		throw std::runtime_error( \
			std::format("ERROR: {} at {}:{}", static_cast<int>(err), __FILE__, __LINE__) \
		); \
	} \
} while (0)