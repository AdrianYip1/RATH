#include "vkBuffer.hpp"

RATH::Buffer::Buffer(Device& device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) :
	_device(device.getDevice()), _size(size),
	buffer(createBuffer(device, size, usage)),
	memory(allocateMemory(device, buffer, memoryProperties))
{

};

RATH::Buffer::~Buffer() {
	vkDestroyBuffer(_device, buffer, nullptr);
	vkFreeMemory(_device, memory, nullptr);
};

void RATH::Buffer::upload(const void* data, VkDeviceSize dataSize) {
	if (dataSize > _size) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> upload(): data larger than buffer");
	}

	void* mapped = nullptr;
	if (vkMapMemory(_device, memory, 0, dataSize, 0, &mapped) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkBuffer.cpp -> upload()");
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
