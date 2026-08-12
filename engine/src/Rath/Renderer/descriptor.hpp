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
			Descriptor(Device& _device, UniformBuffer& _uniform, Storage& _storage);
			~Descriptor();
			Descriptor(const Descriptor& other) = delete;
			Descriptor& operator=(const Descriptor& other) = delete;

			// Returns uniform descriptor set layout
			VkDescriptorSetLayout getUBOSetLayout() const { return uboSetLayout; };
			// Returns sampler (texture) descriptor set layout
			VkDescriptorSetLayout getSamplerLayout() const { return samplerSetLayout; };
			// Returns compute descriptor set layout
			VkDescriptorSetLayout getComputeSetLayout() const { return computeSetLayout; };

			// Returns the uniform descriptor set at a specified frame 
			// TODO/REFACTOR: move getUBOSet and getComputeSet out of descriptor
			// and have the same behaviour as rMaterial
			VkDescriptorSet getUBOSet(u32 frame) const { return uboSets[frame]; }
			// Returns the compute descriptor set at a specified frame 
			// TODO/REFACTOR: move getUBOSet and getComputeSet out of descriptor
			// and have the same behaviour as rMaterial
			VkDescriptorSet getComputeSet(u32 frame) const { return computeSets[frame]; }
			
			// R_DESCRIPTOR_TYPE determines what kind of descriptor set will be made
			// Takes in the pool and setlayout (which work with the passed R_RESCRIPTOR_TYPE)
			// Passes a pointer to texture object 
			// TODO: remove unifrombuffer and storage as members and pass them as pointers
			// aswell, nullptr as defaults
			// Allocates a set per frame in flight, then writes the uniform buffer
			// and the texture view + sampler into them with vkUpdateDescriptorSets
			std::vector<VkDescriptorSet> createDescriptorSets(R_DESCRIPTOR_TYPE type, VkDescriptorPool descriptorPool,
				VkDescriptorSetLayout descriptorSetLayout, Texture* texture);

		private:
			Device& device;
			UniformBuffer& uniform;
			Storage& storage;

			VkDescriptorSetLayout uboSetLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout samplerSetLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout computeSetLayout = VK_NULL_HANDLE;

			VkDescriptorSetLayout createDescriptorSetLayout(R_DESCRIPTOR_TYPE type);

			VkDescriptorPool uboPool = VK_NULL_HANDLE;
			VkDescriptorPool computePool = VK_NULL_HANDLE;
			std::vector<VkDescriptorSet> uboSets;
			std::vector<VkDescriptorSet> computeSets;
	};

	// move somewhere else later
	// General function for creating descriptor pools
	VkDescriptorPool createDescriptorPool(Device& device, R_DESCRIPTOR_TYPE type, u32 setCount);

} // namespace Rath
