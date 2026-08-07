#include "descriptor.hpp"

// Descriptor constructor
Rath::Descriptor::Descriptor(Device& _device, Texture& _texture, UniformBuffer& _uniform, Storage& _storage) :
	device(_device), texture(_texture), uniform(_uniform), storage(_storage) {
	createDescriptorSetLayout();
	createComputeDescriptorSetLayout();
	createDescriptorPool();
	createDescriptorSets();
	createComputeDescriptorSets();
}

// Descriptor destructor
Rath::Descriptor::~Descriptor() {
	vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), descriptorSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), computeDescriptorSetLayout, nullptr);
}

// Creates the descriptor set layout, which has the shape of a descriptor set and
// their binding layout indices used in shaders (and determines which shaders can access it)
void Rath::Descriptor::createDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> layoutBindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pBindings = layoutBindings.data();
	layoutInfo.bindingCount = static_cast<u32>(layoutBindings.size());
	
	if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout");
	}
}

// Creates the descriptor pool for both the uniform and texture
// DescriptorSets are allocated using descriptor pools (they get freed when the pool is destroyed)
// poolSizes determine how many descriptors of each type exist
void Rath::Descriptor::createDescriptorPool() {
	std::array<VkDescriptorPoolSize, 3> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	// Since my graphics and compute descriptors are both allocated from this pool
	// the uniform buffer pool will be used twice
	poolSizes[0].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT) * 2;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT);
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	// 2 storage buffers
	poolSizes[2].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT) * 2;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<u32>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<u32>(MAX_FRAMES_IN_FLIGHT) * 2;

	if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool");
	}
}

// Creates the descriptor sets allocated by the descriptor pools
// descriptorWrites contains buffer and image info, which contain
// a handle to the uniformBuffer and texture sampler + imageView
// Descriptor writes also specifies the binding layout of these
// and vkUpdateDescriptorSets writes descriptorWrites into the set
// the descriptorSet is then binded to the command buffer during rendering
void Rath::Descriptor::createDescriptorSets() {
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
		bufferInfo.buffer = uniform.getUniformBuffer(i);
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture.getTextureImageView();
		imageInfo.sampler = texture.getSampler();

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device.getDevice(), static_cast<u32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void Rath::Descriptor::createComputeDescriptorSetLayout() {
	std::array<VkDescriptorSetLayoutBinding, 3> layoutBindings{};
	layoutBindings[0].binding = 0;
	layoutBindings[0].descriptorCount = 1;
	layoutBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBindings[0].pImmutableSamplers = nullptr;
	layoutBindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	// 2 layout bindings for shader storage buffer objects:
	// 1 to read and 1 to write
	layoutBindings[1].binding = 1;
	layoutBindings[1].descriptorCount = 1;
	layoutBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	layoutBindings[1].pImmutableSamplers = nullptr;
	layoutBindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	layoutBindings[2].binding = 2;
	layoutBindings[2].descriptorCount = 1;
	layoutBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	layoutBindings[2].pImmutableSamplers = nullptr;
	layoutBindings[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<u32>(layoutBindings.size());
	layoutInfo.pBindings = layoutBindings.data();

	if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &computeDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create compute descriptor set layout");
	}
}

void Rath::Descriptor::createComputeDescriptorSets() {
	std::vector<VkDescriptorSetLayout> layout(MAX_FRAMES_IN_FLIGHT, computeDescriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<u32>(layout.size());
	allocInfo.pSetLayouts = layout.data();

	computeDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, computeDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate compute descriptor set");
	}

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

		VkDescriptorBufferInfo uniformBufferInfo{};
		uniformBufferInfo.buffer = uniform.getUniformBuffer(i);
		uniformBufferInfo.offset = 0;
		uniformBufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorBufferInfo storageBufferInfoLastFrame{};
		storageBufferInfoLastFrame.buffer = storage.getStorageBuffer((i - 1) % MAX_FRAMES_IN_FLIGHT);
		storageBufferInfoLastFrame.offset = 0;
		storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

		VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
		storageBufferInfoCurrentFrame.buffer = storage.getStorageBuffer(i);
		storageBufferInfoCurrentFrame.offset = 0;
		storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].dstSet = computeDescriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &uniformBufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].dstSet = computeDescriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[2].dstSet = computeDescriptorSets[i];
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

		vkUpdateDescriptorSets(device.getDevice(), static_cast<u32>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}