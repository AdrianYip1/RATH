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

	const float radius = 1.0f;
	const float speed = 0.5f;

	for (auto& particle : particles) {
		// Uniform point inside a ball: cbrt for radius, acos for even polar spread
		float r = radius * std::cbrt(rndDist(rndEngine));
		float theta = rndDist(rndEngine) * 2.0f * enginemath::PI;
		float phi = std::acos(2.0f * rndDist(rndEngine) - 1.0f);

		float sinPhi = std::sin(phi);
		float nx = sinPhi * std::cos(theta);
		float ny = sinPhi * std::sin(theta);
		float nz = std::cos(phi);

		particle.position = enginemath::Vec4(nx * r, ny * r, nz * r, 1.0f);
		particle.velocity = enginemath::Vec4(nx * speed, ny * speed, nz * speed, 0.0f);
		particle.color = enginemath::Vec4(rndDist(rndEngine), rndDist(rndEngine), rndDist(rndEngine), 1.0f);
	}

	return particles;
}