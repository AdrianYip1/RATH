#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "vkDevice.hpp"

namespace RATH {
	class Buffer {
	public:
		Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
		~Buffer();
		Buffer(const Buffer& other) = delete;
		Buffer& operator=(const Buffer& other) = delete;

		// Copies raw bytes into the buffer (memory must be HOST_VISIBLE)
		void upload(const void* data, VkDeviceSize dataSize);

		VkBuffer getBuffer() { return buffer; };
		VkDeviceSize getSize() { return _size; };

	private:
		VkDevice _device = VK_NULL_HANDLE;
		VkDeviceSize _size = 0;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;

		static VkBuffer createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);
		static VkDeviceMemory allocateMemory(Device& device, VkBuffer buffer, VkMemoryPropertyFlags memoryProperties);
		static u32 findMemoryType(VkPhysicalDevice physDevice, u32 typeFilter, VkMemoryPropertyFlags properties);
	};
} // namespace RATH
