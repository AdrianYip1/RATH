#include "vertex.hpp"

// Vertex buffer constructor
Rath::VertexBuffer::VertexBuffer(Device& _device, Buffer& _buffer) :
	device(_device),
	buffer(_buffer) {
	createVertexBuffer();
	createIndexBuffer();
}

// Vertex buffer destructor
Rath::VertexBuffer::~VertexBuffer() {
	vkDestroyBuffer(device.getDevice(), indexBuffer, nullptr);
	vkFreeMemory(device.getDevice(), indexBufferMemory, nullptr);

	vkDestroyBuffer(device.getDevice(), vertexBuffer, nullptr);
	vkFreeMemory(device.getDevice(), vertexBufferMemory, nullptr);
}

// Wrapper for creating the vertex buffer
// Uses a staging buffer first before copying vertices data into the actual
// vertexBuffer (which holds the GPU optimized memory) -> staging buffer is destroyed after
// Both the staging and vertex buffers call the createBuffer method
void Rath::VertexBuffer::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size) bufferSize);
	vkUnmapMemory(device.getDevice(), stagingBufferMemory);

	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
						VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						vertexBuffer, vertexBufferMemory);

	buffer.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

// Wrapper for creating the index buffer
// Staging buffer used here before copying indices data into
// indexBuffer (which holds the GPU optimized memory) -> staging buffer destroyed after
// Both the staging and indices buffers call createBuffer method
void Rath::VertexBuffer::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
						stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size) bufferSize);
	vkUnmapMemory(device.getDevice(), stagingBufferMemory);

	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
						VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
						VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
						indexBuffer, indexBufferMemory);
	buffer.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
	
}