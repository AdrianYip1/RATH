#include "uniform.hpp"

// Uniform buffer constructor
Rath::UniformBuffer::UniformBuffer(Device& _device, Swapchain& _swapchain, 
								   Buffer& _buffer, Camera& _camera) :
	device(_device), swapchain(_swapchain), buffer(_buffer), camera(_camera) {
	createUniformBuffers();
}

// Uniform buffer destructor
Rath::UniformBuffer::~UniformBuffer() {
	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device.getDevice(), uniformBuffers[i], nullptr);
		vkFreeMemory(device.getDevice(), uniformBuffersMemory[i], nullptr);
	}
}

// Updates the uniform buffer
// This is done by changing the values within the ubo struct and copying
// the data into uniformBuffersMapped at the current frame
// This is possible since uniformBufferMapped is persistently mapped
// to the uniformBuffer's memory
void Rath::UniformBuffer::updateUniformBuffer(u32 currentImage) {
	
	f32 deltaTime = camera.getDeltaTime();
	f32 time = camera.getElapsedTime();

	UniformBufferObject ubo{
		enginemath::Mat4::identity(),
		enginemath::Mat4::identity(),
		deltaTime
	};
	ubo.view = camera.getView();
	ubo.proj = camera.getProj();

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

// Creates an array of uniformBuffers, memory, then persistently maps them into
// uniformBuffersMapped
// A uniformBuffer is required for each frame in flight as multiple frames
// may be in flight -> dont want to overwrite buffers 
void Rath::UniformBuffer::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							uniformBuffers[i], uniformBuffersMemory[i]);

		// Persistent mapping
		vkMapMemory(device.getDevice(), uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

