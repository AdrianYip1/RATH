#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/vec2.hpp>
#include <enginemath/vec3.hpp>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../device.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Buffer {
		public:
			Buffer(Device& _device);
			~Buffer();
			Buffer(const Buffer& other) = delete;
			Buffer& operator=(const Buffer& other) = delete;

			void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
							  VkMemoryPropertyFlags properties, VkBuffer& buffer,
							  VkDeviceMemory& bufferMemory);

			void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
			u32 findMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);

			VkCommandBuffer beginSingleTimeCommands();
			void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	private:
			Device& device;
			
	};
} // namespace Rath