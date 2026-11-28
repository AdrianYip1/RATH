#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "vkDevice.hpp"

#include "RATH_VULKAN_RHI_MACROS.hpp"

namespace RATH {
	class Buffer {
	public:
		Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties);
		~Buffer();
		Buffer(const Buffer& other) = delete;
		Buffer& operator=(const Buffer& other) = delete;

		// HOST_VISIBLE memory -> mapped + memcpy directly
		// otherwise (DEVICE_LOCAL) -> temporary staging buffer + GPU copy (buffer needs TRANSFER_DST usage)
		void upload(const void* data, VkDeviceSize dataSize, VkDeviceSize dstOffset = 0);

		// GPU side copy between any two buffers (src needs TRANSFER_SRC, dst needs TRANSFER_DST), blocks until done
		static void copyBuffer(Device& device, VkBuffer src, VkBuffer dst, VkDeviceSize size,
							   VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0);

		VkBuffer getBuffer() { return buffer; };
		VkDeviceSize getSize() { return _size; };

	private:
		Device& _device;
		VkDeviceSize _size = 0;
		VkBufferUsageFlags _usage = 0;
		VkMemoryPropertyFlags _memoryProperties = 0;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;

		void writeMapped(const void* data, VkDeviceSize dataSize, VkDeviceSize dstOffset);

		static VkBuffer createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage);
		static VkDeviceMemory allocateMemory(Device& device, VkBuffer buffer, VkMemoryPropertyFlags memoryProperties);
		static u32 findMemoryType(VkPhysicalDevice physDevice, u32 typeFilter, VkMemoryPropertyFlags properties);
	};
} // namespace RATH
