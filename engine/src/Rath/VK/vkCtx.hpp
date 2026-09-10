#pragma once
#include <vulkan/vulkan.h>

namespace RATH {
	struct vkCtx {
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physical = VK_NULL_HANDLE;
		VkDevice device = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};
} // namespace RATH