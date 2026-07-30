#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/mathutils.hpp>
#include <enginemath/vec2.hpp>
#include <enginemath/vec3.hpp>
#include <enginemath/mat4.hpp>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../device.hpp"
#include "../swapchain.hpp"
#include "../images/image.hpp"
#include "buffer.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <chrono>

namespace Rath {
	struct UniformBufferObject {
		enginemath::Mat4 model;
		enginemath::Mat4 view;
		enginemath::Mat4 proj;
	};

	class UniformBuffer {
		public:
			UniformBuffer(Device& _device, Swapchain& _swapchain, Buffer& _buffer, Image& _image);
			~UniformBuffer();
			UniformBuffer(const UniformBuffer& other) = delete;
			UniformBuffer& operator=(const UniformBuffer& other) = delete;
	
			const VkDescriptorSetLayout getDescriptorSetLayout();
			void updateUniformBuffer(u32 currentImage);
			std::vector<VkDescriptorSet> getDescriptorSets();
		private:
			Device& device;
			Swapchain& swapchain;
			Buffer& buffer;
			Image& image;

			VkDescriptorSetLayout descriptorSetLayout;
			VkDescriptorPool descriptorPool;
			std::vector<VkDescriptorSet> descriptorSets;
			// New data is copied into uniform buffer every frame
			// no point in having staging buffer
			// Should use multiple buffers as multiple frames
			// may be in flight -> dont want to overwrite buffers 
			std::vector<VkBuffer> uniformBuffers;
			std::vector<VkDeviceMemory> uniformBuffersMemory;
			std::vector<void*> uniformBuffersMapped;
			

			void createDescriptorSetLayout();
			void createUniformBuffers();
			void createDescriptorPool();
			void createDescriptorSets();

	};

} // namespace Rath