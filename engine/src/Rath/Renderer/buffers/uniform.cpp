#include "uniform.hpp"

Rath::UniformBuffer::UniformBuffer(Device& _device, Swapchain& _swapchain, Buffer& _buffer) :
	device(_device), buffer(_buffer), swapchain(_swapchain) {
	createDescriptorSetLayout();
	std::cout << "Created descriptor set layout" << std::endl;
	createUniformBuffers();
	std::cout << "Created uniform buffers" << std::endl;
	createDescriptorPool();
	std::cout << "Created descriptor pool" << std::endl;
	createDescriptorSets();
	std::cout << "Created descriptor sets" << std::endl;
}

Rath::UniformBuffer::~UniformBuffer() {
	vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
	std::cout << "Destroyed descriptor pools" << std::endl;

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device.getDevice(), uniformBuffers[i], nullptr);
		vkFreeMemory(device.getDevice(), uniformBuffersMemory[i], nullptr);
	}
	vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);
	std::cout << "Destroyed descriptor set layout" << std::endl;
}


const VkDescriptorSetLayout Rath::UniformBuffer::getDescriptorSetLayout() {
	return descriptorSetLayout;
}

void Rath::UniformBuffer::updateUniformBuffer(u32 currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	
	UniformBufferObject ubo{
		enginemath::Mat4::identity(),
		enginemath::Mat4::identity(),
		enginemath::Mat4::identity()
	};
	ubo.model = enginemath::Mat4::rotateZ(enginemath::toRad(std::sin(time)) * 90.0f);
	ubo.view = enginemath::Mat4::lookAtM(enginemath::Vec3(2.0f, 2.0f, 2.0f), 
										 enginemath::Vec3(0.0f, 0.0f, 0.0f), 
										 enginemath::Vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = enginemath::Mat4::projectionM(enginemath::toRad(45.0f), 
									   	 swapchain.getExtent().width / (f32) swapchain.getExtent().height,
										 0.1f, 10.0f);

	// Vulkan's Y coord in clip space is inverted compared to OpenGL's
	ubo.proj.m[1][1] *= -1; // or ubo.proj.col1.y *= -1;

	memcpy(uniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
}

std::vector<VkDescriptorSet> Rath::UniformBuffer::getDescriptorSets() {
	return descriptorSets;
}


void Rath::UniformBuffer::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

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

void Rath::UniformBuffer::createDescriptorPool() {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<u32>(MAX_FRAMES_IN_FLIGHT);

	if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

void Rath::UniformBuffer::createDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor sets");
	}

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		// Assigns bufferInfo to descriptorSets[i]
		vkUpdateDescriptorSets(device.getDevice(), 1, &descriptorWrite, 0, nullptr);
	}
}