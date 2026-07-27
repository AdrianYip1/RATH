#include "vertex.hpp"

Rath::VertexBuffer::VertexBuffer(Device& _device) :
	device(_device) {
	createVertexBuffer();
}

Rath::VertexBuffer::~VertexBuffer() {
	vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
	vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
}

VkBuffer Rath::VertexBuffer::getVertexBuffer() {
	return vertexBuffer;
}

void Rath::VertexBuffer::createVertexBuffer() {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(vertices[0]) * vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device.getDevice(), &bufferInfo, nullptr, &vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create vertex buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device.getDevice(), vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, 
											   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
											   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(device.getDevice(), &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate vertex buffer memory");
	}

	vkBindBufferMemory(device.getDevice(), vertexBuffer, vertexBufferMemory, 0);
	
	void* data;
	vkMapMemory(device.getDevice(), vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertices.data(), (size)bufferInfo.size);
	vkUnmapMemory(device.getDevice(), vertexBufferMemory);
}

Rath::u32 Rath::VertexBuffer::findMemoryType(u32 typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(device.getPhysicalDevice(), &memProperties);

	for (u32 i = 0; i < memProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}
	throw std::runtime_error("Failed to find suitable memory type");
}