#include "vkBuffer.hpp"

RATH::Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) :
	_device(device.getDevice()), _size(size), _usage(usage), _memoryProperties(memoryProperties),
	buffer(createBuffer(device, size, usage)),
	memory(allocateMemory(device, buffer, memoryProperties))
{

};

RATH::Buffer::~Buffer() {
	vkDestroyBuffer(_device, buffer, nullptr);
	vkFreeMemory(_device, memory, nullptr);
};

void RATH::Buffer::upload(Device& device, const void* data, VkDeviceSize dataSize, VkDeviceSize dstOffset) {
	if (dstOffset + dataSize > _size) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> upload(): data larger than buffer");
	}

	// CPU can write it directly
	if (_memoryProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
		writeMapped(data, dataSize, dstOffset);
		return;
	}

	if (!(_usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT)) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> upload(): device local buffer is missing TRANSFER_DST usage");
	}

	Buffer staging(device, dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
				   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	staging.writeMapped(data, dataSize, 0);

	copyBuffer(device, staging.getBuffer(), buffer, dataSize, 0, dstOffset);
}

void RATH::Buffer::copyBuffer(Device& device, VkBuffer src, VkBuffer dst, VkDeviceSize size,
							  VkDeviceSize srcOffset, VkDeviceSize dstOffset) {
	VkCommandBuffer commandBuffer = device.beginSingleTimeCommands();

	VkBufferCopy copyRegion = {
		.srcOffset = srcOffset,
		.dstOffset = dstOffset,
		.size = size
	};
	vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyRegion);

	device.endSingleTimeCommands(commandBuffer);
}

void RATH::Buffer::writeMapped(const void* data, VkDeviceSize dataSize, VkDeviceSize dstOffset) {
	void* mapped = nullptr;
	if (vkMapMemory(_device, memory, dstOffset, dataSize, 0, &mapped) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> writeMapped()");
	}
	memcpy(mapped, data, static_cast<size>(dataSize));
	vkUnmapMemory(_device, memory);
}

VkBuffer RATH::Buffer::createBuffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage) {
	VkBufferCreateInfo bufferCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	VkBuffer buffer = VK_NULL_HANDLE;

	if (vkCreateBuffer(device.getDevice(), &bufferCreateInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> createBuffer()");
	}

	return buffer;
}

VkDeviceMemory RATH::Buffer::allocateMemory(Device& device, VkBuffer buffer, VkMemoryPropertyFlags memoryProperties) {
	VkMemoryRequirements memReq;
	vkGetBufferMemoryRequirements(device.getDevice(), buffer, &memReq);

	VkMemoryAllocateInfo allocInfo{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = memReq.size,
		.memoryTypeIndex = findMemoryType(device.getPhysicalDevice(), memReq.memoryTypeBits, memoryProperties)
	};

	VkDeviceMemory memory = VK_NULL_HANDLE;

	if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> allocateMemory()");
	}

	vkBindBufferMemory(device.getDevice(), buffer, memory, 0);

	return memory;
}

RATH::u32 RATH::Buffer::findMemoryType(VkPhysicalDevice physDevice, u32 typeFilter, VkMemoryPropertyFlags properties) {
	// Available types of memory
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physDevice, &memProperties);

	for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("ERROR: vkBuffer.cpp -> findMemoryType()");
}
