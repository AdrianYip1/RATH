#include "descriptor.hpp"

// Descriptor constructor
Rath::Descriptor::Descriptor(Device& _device, UniformBuffer& _uniform, R_Light& _light, Storage& _storage) :
	device(_device), uniform(_uniform), light(_light), storage(_storage) {

	uboSetLayout = createDescriptorSetLayout(R_DESCRIPTOR_TYPE::R_UNIFORM);
	samplerSetLayout = createDescriptorSetLayout(R_DESCRIPTOR_TYPE::R_SAMPLER);
	computeSetLayout = createDescriptorSetLayout(R_DESCRIPTOR_TYPE::R_COMPUTE);

	// REFACTOR: remove these and add them into material
	uboPool = createDescriptorPool(device, R_DESCRIPTOR_TYPE::R_UNIFORM, MAX_FRAMES_IN_FLIGHT);
	computePool = createDescriptorPool(device, R_DESCRIPTOR_TYPE::R_COMPUTE, MAX_FRAMES_IN_FLIGHT);

	uboSets = createDescriptorSets(R_DESCRIPTOR_TYPE::R_UNIFORM, uboPool, uboSetLayout, nullptr);
	computeSets = createDescriptorSets(R_DESCRIPTOR_TYPE::R_COMPUTE, computePool, computeSetLayout, nullptr);
}

// Descriptor destructor
Rath::Descriptor::~Descriptor() {
	vkDestroyDescriptorPool(device.getDevice(), uboPool, nullptr);
	vkDestroyDescriptorPool(device.getDevice(), computePool, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), uboSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), samplerSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), computeSetLayout, nullptr);
}

// Creates and returns a descriptor set layout depending on R_DESCRIPTOR_TYPE
VkDescriptorSetLayout Rath::Descriptor::createDescriptorSetLayout(R_DESCRIPTOR_TYPE type) {
	std::array<VkDescriptorSetLayoutBinding, 3> layoutBinding{};

	u32 bindingCount = 1;
	if (type == R_DESCRIPTOR_TYPE::R_UNIFORM) bindingCount = 2;
	if (type == R_DESCRIPTOR_TYPE::R_COMPUTE) bindingCount = 3;

	switch (type) {
		case R_DESCRIPTOR_TYPE::R_SAMPLER: {
			layoutBinding[0].binding = 0;
			layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			layoutBinding[0].descriptorCount = 1;
			layoutBinding[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			layoutBinding[0].pImmutableSamplers = nullptr;

			break;
		}

		case R_DESCRIPTOR_TYPE::R_UNIFORM: {
			layoutBinding[0].binding = 0;
			layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding[0].descriptorCount = 1;
			layoutBinding[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			layoutBinding[0].pImmutableSamplers = nullptr;

			layoutBinding[1].binding = 1;
			layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding[1].descriptorCount = 1;
			layoutBinding[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			layoutBinding[1].pImmutableSamplers = nullptr;

			break;
		}
		case R_DESCRIPTOR_TYPE::R_COMPUTE: {
			layoutBinding[0].binding = 0;
			layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			layoutBinding[0].descriptorCount = 1;
			layoutBinding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			layoutBinding[0].pImmutableSamplers = nullptr;

			layoutBinding[1].binding = 1;
			layoutBinding[1].descriptorCount = 1;
			layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding[1].pImmutableSamplers = nullptr;
			layoutBinding[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			layoutBinding[2].binding = 2;
			layoutBinding[2].descriptorCount = 1;
			layoutBinding[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding[2].pImmutableSamplers = nullptr;
			layoutBinding[2].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			break;
		}
	}
	
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.pBindings = layoutBinding.data();
	layoutInfo.bindingCount = bindingCount;

	VkDescriptorSetLayout descriptorSetLayout;

	if (vkCreateDescriptorSetLayout(device.getDevice(), &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor set layout");
	}

	return descriptorSetLayout;
}

// Creates the descriptor sets allocated by the descriptor pools
// descriptorWrites contains buffer and image info, which contain
// a handle to the uniformBuffer and texture sampler + imageView
// Descriptor writes also specifies the binding layout of these
// and vkUpdateDescriptorSets writes descriptorWrites into the set
// the descriptorSet is then binded to the command buffer during rendering
std::vector<VkDescriptorSet> Rath::Descriptor::createDescriptorSets(R_DESCRIPTOR_TYPE type, 
	VkDescriptorPool descriptorPool, VkDescriptorSetLayout descriptorSetLayout, Texture* texture) {

	// Compute and uniform need a set per frame
	const u32 setCount = (type == R_DESCRIPTOR_TYPE::R_SAMPLER ? 1 : MAX_FRAMES_IN_FLIGHT);

	std::vector<VkDescriptorSetLayout> layouts(setCount, descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<u32>(setCount);
	allocInfo.pSetLayouts = layouts.data();

	// creates a vector of setCount size for the descriptor set
	// which gets returned at the end after it is updated with the writes
	std::vector<VkDescriptorSet> sets(setCount);
	if (vkAllocateDescriptorSets(device.getDevice(), &allocInfo, sets.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate descriptor sets");
	}

	for (size i = 0; i < setCount; i++) {

		std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

		// Chooses functionality based on the R_DESCRIPTOR_TYPE
		switch (type) {
			case R_DESCRIPTOR_TYPE::R_UNIFORM: {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniform.getUniformBuffer(i);
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);

				VkDescriptorBufferInfo lightBufferInfo{};
				lightBufferInfo.buffer = light.getLightBuffer(i);
				lightBufferInfo.offset = 0;
				lightBufferInfo.range = sizeof(R_LightUbo);

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = sets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].dstSet = sets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = &lightBufferInfo;

				vkUpdateDescriptorSets(device.getDevice(), 2, descriptorWrites.data(), 0, nullptr);
				break;
			}

			case R_DESCRIPTOR_TYPE::R_SAMPLER: {
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture->getTextureImageView();
				imageInfo.sampler = texture->getSampler();

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = sets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pImageInfo = &imageInfo;

				vkUpdateDescriptorSets(device.getDevice(), 1, descriptorWrites.data(), 0, nullptr);
				break;
			}

			case R_DESCRIPTOR_TYPE::R_COMPUTE: {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniform.getUniformBuffer(i);
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);

				VkDescriptorBufferInfo storageBufferInfoLastFrame{};
				storageBufferInfoLastFrame.buffer = storage.getStorageBuffer((i - 1) % MAX_FRAMES_IN_FLIGHT);
				storageBufferInfoLastFrame.offset = 0;
				storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

				VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
				storageBufferInfoCurrentFrame.buffer = storage.getStorageBuffer(i);
				storageBufferInfoCurrentFrame.offset = 0;
				storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = sets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[1].dstSet = sets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = &storageBufferInfoLastFrame;

				descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[2].dstSet = sets[i];
				descriptorWrites[2].dstBinding = 2;
				descriptorWrites[2].dstArrayElement = 0;
				descriptorWrites[2].descriptorCount = 1;
				descriptorWrites[2].pBufferInfo = &storageBufferInfoCurrentFrame;

				vkUpdateDescriptorSets(device.getDevice(), 3, descriptorWrites.data(), 0, nullptr);
				break;
			}
		}
	}

	return sets;
}

// Creates the descriptor pool for both the uniform and texture
// DescriptorSets are allocated using descriptor pools (they get freed when the pool is destroyed)
// poolSizes determine how many descriptors of each type exist
VkDescriptorPool Rath::createDescriptorPool(Device& device, R_DESCRIPTOR_TYPE type, u32 setCount) {
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	const u32 poolSizeCount = (type == R_DESCRIPTOR_TYPE::R_COMPUTE ? 2 : 1);

	switch (type) {
		case R_DESCRIPTOR_TYPE::R_SAMPLER: {
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			poolSizes[0].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT);

			break;
		}

		case R_DESCRIPTOR_TYPE::R_UNIFORM: {
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT) * 2;

			break;
		}

		case R_DESCRIPTOR_TYPE::R_COMPUTE: {
			poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSizes[0].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT);
			poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			// 2 storage buffers
			poolSizes[1].descriptorCount = static_cast<u32>(MAX_FRAMES_IN_FLIGHT) * 2;

			break;
		}
	}

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = poolSizeCount;
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = setCount;

	VkDescriptorPool descriptorPool;

	if (vkCreateDescriptorPool(device.getDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create descriptor pool");
	}

	return descriptorPool;
}
