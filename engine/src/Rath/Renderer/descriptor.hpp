#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/mathutils.hpp>
#include <enginemath/vec2.hpp>
#include <enginemath/vec3.hpp>
#include <enginemath/mat4.hpp>

// Rath files
#include "Rath/Core/defines.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "buffers/buffer.hpp"
#include "buffers/uniform.hpp"
#include "images/texture.hpp"
#include "buffers/storage.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Descriptor {
		public:
			Descriptor(Device& _device, Texture& _texture, UniformBuffer& _uniform, Storage& _storage);
			~Descriptor();
			Descriptor(const Descriptor& other) = delete;
			Descriptor& operator=(const Descriptor& other) = delete;

			// Returns descriptorSetLayouts
			VkDescriptorSetLayout getUBOSetLayout() const { return uboSetLayout; };
			VkDescriptorSetLayout getSamplerLayout() const { return samplerSetLayout; };
			VkDescriptorSetLayout getComputeSetLayout() const { return computeSetLayout; };

			// Returns descriptorSets at frame
			VkDescriptorSet getDescriptorSet(u32 frame) const { return descriptorSets[frame]; };


		private:
			Device& device;
			Texture& texture;
			UniformBuffer& uniform;
			Storage& storage;

			VkDescriptorSetLayout uboSetLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout samplerSetLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout computeSetLayout = VK_NULL_HANDLE;

			VkDescriptorPool descriptorPool;
			std::vector<VkDescriptorSet> descriptorSets;

			// Creates the pool the sets are allocated from, poolSizes determines
			// how many descriptors of each type exist
			void createDescriptorPool();

			// Allocates a set per frame in flight, then writes the uniform buffer
			// and the texture view + sampler into them with vkUpdateDescriptorSets
			void createDescriptorSets(R_DESCRIPTOR_TYPE type, VkDescriptorSetLayout descriptorSetLayout);

			VkDescriptorSetLayout createDescriptorSetLayout(R_DESCRIPTOR_TYPE type);
			
	};
} // namespace Rath
