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

			// Creates the buffer and bufferMemory, storing them in the passed in references
			// Also binds the buffer memory to the buffer
			void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
							  VkMemoryPropertyFlags properties, VkBuffer& buffer,
							  VkDeviceMemory& bufferMemory);

			// Copies one buffer to another in a command buffer
			void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

			// Returns the index of a memory type that is set in typeFilter and
			// has all of the desired properties
			u32 findMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties);

			// Returns a throwaway primary command buffer that has started recording
			// ONE_TIME_SUBMIT as it is submitted once then freed
			VkCommandBuffer beginSingleTimeCommands();

			// Ends recording, submits to the graphics queue, then blocks until the GPU is done
			void endSingleTimeCommands(VkCommandBuffer commandBuffer);

	private:
			Device& device;
			
	};
} // namespace Rath