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

			// Returns descriptorSetLayout
			VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; };

			// Returns descriptorSets at frame
			VkDescriptorSet getDescriptorSet(u32 frame) const { return descriptorSets[frame]; };
		
			// Returns computeDescriptorSetLayout
			VkDescriptorSetLayout getComputeDescriptorSetLayout() const { return computeDescriptorSetLayout; };

			// Returns computeDescriptorSets at frame
			VkDescriptorSet getComputeDescriptorSet(u32 frame) const { return computeDescriptorSets[frame]; };

		private:
			Device& device;
			Texture& texture;
			UniformBuffer& uniform;
			Storage& storage;

			VkDescriptorSetLayout descriptorSetLayout;
			VkDescriptorPool descriptorPool;
			std::vector<VkDescriptorSet> descriptorSets;

			VkDescriptorSetLayout computeDescriptorSetLayout;
			std::vector<VkDescriptorSet> computeDescriptorSets;

			// Creates the shape of a descriptor set: what type sits at each binding
			// number and which shader stages can see it
			void createDescriptorSetLayout();

			// Creates the pool the sets are allocated from, poolSizes determines
			// how many descriptors of each type exist
			void createDescriptorPool();

			// Allocates a set per frame in flight, then writes the uniform buffer
			// and the texture view + sampler into them with vkUpdateDescriptorSets
			void createDescriptorSets();

			void createComputeDescriptorSetLayout();

			void createComputeDescriptorSets();
			
	};
} // namespace Rath
