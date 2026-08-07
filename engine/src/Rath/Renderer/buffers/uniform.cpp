#include "uniform.hpp"

// Uniform buffer constructor
Rath::UniformBuffer::UniformBuffer(Device& _device, Swapchain& _swapchain, Buffer& _buffer) :
	device(_device), swapchain(_swapchain), buffer(_buffer) {
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
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	UniformBufferObject ubo{
		enginemath::Mat4::identity(),
		enginemath::Mat4::identity(),
		enginemath::Mat4::identity(),
		time
	};
	ubo.model = enginemath::Mat4::rotateZ(enginemath::toRad(std::sin(time)) * 90.0f);
	ubo.view = enginemath::Mat4::lookAtM(enginemath::Vec3(2.0f, 2.0f, 2.0f), 
										 enginemath::Vec3(0.0f, 0.0f, 0.0f), 
										 enginemath::Vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = enginemath::Mat4::projectionM(enginemath::toRad(45.0f), 
									   	 swapchain.getExtent().width / (f32) swapchain.getExtent().height,
										 0.1f, 10.0f);

	// Vulkan's Y coord in clip space is inverted compared to OpenGL's
	ubo.proj.m[1][1] *= -1; // or ubo.proj.col1.y *= -1; per enginemath's conventions

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

