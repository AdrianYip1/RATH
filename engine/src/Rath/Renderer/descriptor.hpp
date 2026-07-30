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

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Descriptor {
		public:
			Descriptor(Device& _device, Texture& _texture, UniformBuffer& _uniform);
			~Descriptor();
			Descriptor(const Descriptor& other) = delete;
			Descriptor& operator=(const Descriptor& other) = delete;

			VkDescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; };
			VkDescriptorSet getDescriptorSet(u32 frame) { return descriptorSets[frame]; };
		private:
			Device& device;
			Texture& texture;
			UniformBuffer& uniform;
			

			VkDescriptorSetLayout descriptorSetLayout;
			VkDescriptorPool descriptorPool;
			std::vector<VkDescriptorSet> descriptorSets;

			void createDescriptorSetLayout();
			void createDescriptorPool();
			void createDescriptorSets();
			
	};
} // namespace Rath
