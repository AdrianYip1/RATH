#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

namespace RATH {
	class Descriptor {
	public:
		Descriptor();
		~Descriptor();
		Descriptor(const Descriptor& other) = delete;
		Descriptor& operator=(const Descriptor& other) = delete;

	private:
		VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;

		static VkDescriptorSetLayout createDescriptorSetLayout();
	};
} // namespace RATH