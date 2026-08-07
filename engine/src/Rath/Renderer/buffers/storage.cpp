#include "storage.hpp"

// Storage constructor
Rath::Storage::Storage(Device& _device, Buffer& _buffer) : 
	device(_device), buffer(_buffer) {
	createStorageBuffer();
}

// Storage destructor
Rath::Storage::~Storage() {
	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device.getDevice(), shaderStorageBuffers[i], nullptr);
		vkFreeMemory(device.getDevice(), shaderStorageBuffersMemory[i], nullptr);
	}
}

void Rath::Storage::createStorageBuffer() {
	VkDeviceSize bufferSize = sizeof(Particle) * PARTICLE_COUNT;

	shaderStorageBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	shaderStorageBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, initParticles().data(), bufferSize);
	vkUnmapMemory(device.getDevice(), stagingBufferMemory);
	
	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
							VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
							shaderStorageBuffers[i], shaderStorageBuffersMemory[i]);
		buffer.copyBuffer(stagingBuffer, shaderStorageBuffers[i], bufferSize);
	}

	vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

std::vector<Rath::Particle> Rath::Storage::initParticles() {
	std::default_random_engine rndEngine((unsigned)time(nullptr));
	std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);

	std::vector<Particle> particles(PARTICLE_COUNT);

	for (auto& particle : particles) {
		float r = 0.25f * sqrt(rndDist(rndEngine));
		float theta = rndDist(rndEngine) * 2 * enginemath::PI;
		float x = r * std::cos(theta) * 1600 * 1200;
		float y = r * std::sin(theta);
		particle.position = enginemath::Vec2(x, y);
		particle.velocity = enginemath::Vec2(x, y).normalized() * 0.00025f;
		particle.color = enginemath::Vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
	}

	return particles;
}