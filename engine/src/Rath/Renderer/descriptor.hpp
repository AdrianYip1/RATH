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
#include "scenes/light.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Descriptor {
		public:
			Descriptor(Device& _device, UniformBuffer& _uniform, R_Light& _light);
			~Descriptor();
			Descriptor(const Descriptor& other) = delete;
			Descriptor& operator=(const Descriptor& other) = delete;

			// Returns uniform descriptor set layout
			VkDescriptorSetLayout getUBOSetLayout() const { return uboSetLayout; };
			// Returns sampler (texture) descriptor set layout
			VkDescriptorSetLayout getSamplerLayout() const { return samplerSetLayout; };

			// Returns the uniform descriptor set at a specified frame
			// TODO/REFACTOR: move getUBOSet out of descriptor
			// and have the same behaviour as rMaterial
			VkDescriptorSet getUBOSet(u32 frame) const { return uboSets[frame]; }

			// R_DESCRIPTOR_TYPE determines what kind of descriptor set will be made
			// Takes in the pool and setlayout (which work with the passed R_RESCRIPTOR_TYPE)
			// Passes a pointer to texture object 
			// TODO: remove uniformbuffer as a member and pass it as a pointer
			// aswell, nullptr as default
			// Allocates a set per frame in flight, then writes the uniform buffer
			// and the texture view + sampler into them with vkUpdateDescriptorSets
			std::vector<VkDescriptorSet> createDescriptorSets(R_DESCRIPTOR_TYPE type, VkDescriptorPool descriptorPool,
				VkDescriptorSetLayout descriptorSetLayout, Texture* texture);

		private:
			Device& device;
			UniformBuffer& uniform;
			R_Light& light;

			VkDescriptorSetLayout uboSetLayout = VK_NULL_HANDLE;
			VkDescriptorSetLayout samplerSetLayout = VK_NULL_HANDLE;

			VkDescriptorSetLayout createDescriptorSetLayout(R_DESCRIPTOR_TYPE type);

			VkDescriptorPool uboPool = VK_NULL_HANDLE;
			std::vector<VkDescriptorSet> uboSets;
	};

	// move somewhere else later
	// General function for creating descriptor pools
	VkDescriptorPool createDescriptorPool(Device& device, R_DESCRIPTOR_TYPE type, u32 setCount);

} // namespace Rath
