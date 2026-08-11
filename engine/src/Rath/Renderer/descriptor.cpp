#include "descriptor.hpp"

// Descriptor constructor
Rath::Descriptor::Descriptor(Device& _device, Texture& _texture, UniformBuffer& _uniform, Storage& _storage) :
	device(_device), texture(_texture), uniform(_uniform), storage(_storage) {

	uboSetLayout = createDescriptorSetLayout(R_DESCRIPTOR_TYPE::R_UNIFORM);
	samplerSetLayout = createDescriptorSetLayout(R_DESCRIPTOR_TYPE::R_SAMPLER);
	computeSetLayout = createDescriptorSetLayout(R_DESCRIPTOR_TYPE::R_COMPUTE);
}

// Descriptor destructor
Rath::Descriptor::~Descriptor() {
	vkDestroyDescriptorPool(device.getDevice(), descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), uboSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), samplerSetLayout, nullptr);
	vkDestroyDescriptorSetLayout(device.getDevice(), computeSetLayout, nullptr);
}

VkDescriptorSetLayout Rath::Descriptor::createDescriptorSetLayout(R_DESCRIPTOR_TYPE type) {
	std::array<VkDescriptorSetLayoutBinding, 2> layoutBinding{};

	const u32 bindingCount = (type == R_DESCRIPTOR_TYPE::R_COMPUTE ? 2 : 1);

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

			break;
		}
		case R_DESCRIPTOR_TYPE::R_COMPUTE: {
			layoutBinding[0].binding = 0;
			layoutBinding[0].descriptorCount = 1;
			layoutBinding[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding[0].pImmutableSamplers = nullptr;
			layoutBinding[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

			layoutBinding[1].binding = 1;
			layoutBinding[1].descriptorCount = 1;
			layoutBinding[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			layoutBinding[1].pImmutableSamplers = nullptr;
			layoutBinding[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

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
void Rath::Descriptor::createDescriptorSets(R_DESCRIPTOR_TYPE type, VkDescriptorSetLayout descriptorSetLayout) {
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

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		switch (type) {
			case R_DESCRIPTOR_TYPE::R_UNIFORM: {
				VkDescriptorBufferInfo bufferInfo{};
				bufferInfo.buffer = uniform.getUniformBuffer(i);
				bufferInfo.offset = 0;
				bufferInfo.range = sizeof(UniformBufferObject);

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &bufferInfo;


				vkUpdateDescriptorSets(device.getDevice(), 1, descriptorWrites.data(), 0, nullptr);
				break;
			}

			case R_DESCRIPTOR_TYPE::R_SAMPLER: {
				VkDescriptorImageInfo imageInfo{};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = texture.getTextureImageView();
				imageInfo.sampler = texture.getSampler();

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].dstSet = descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pImageInfo = &imageInfo;


				vkUpdateDescriptorSets(device.getDevice(), 1, descriptorWrites.data(), 0, nullptr);
				break;
			}

			case R_DESCRIPTOR_TYPE::R_COMPUTE: {
				VkDescriptorBufferInfo storageBufferInfoLastFrame{};
				storageBufferInfoLastFrame.buffer = storage.getStorageBuffer((i - 1) % MAX_FRAMES_IN_FLIGHT);
				storageBufferInfoLastFrame.offset = 0;
				storageBufferInfoLastFrame.range = sizeof(Particle) * PARTICLE_COUNT;

				VkDescriptorBufferInfo storageBufferInfoCurrentFrame{};
				storageBufferInfoCurrentFrame.buffer = storage.getStorageBuffer(i);
				storageBufferInfoCurrentFrame.offset = 0;
				storageBufferInfoCurrentFrame.range = sizeof(Particle) * PARTICLE_COUNT;

				descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[0].dstSet = descriptorSets[i];
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pBufferInfo = &storageBufferInfoLastFrame;

				descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
				descriptorWrites[1].dstSet = descriptorSets[i];
				descriptorWrites[1].dstBinding = 1;
				descriptorWrites[1].dstArrayElement = 0;
				descriptorWrites[1].descriptorCount = 1;
				descriptorWrites[1].pBufferInfo = &storageBufferInfoCurrentFrame;


				vkUpdateDescriptorSets(device.getDevice(), 2, descriptorWrites.data(), 0, nullptr);
				break;
			}
		}
	}
}
