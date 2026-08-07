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
#include "buffer.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <random>
#include <array>

namespace Rath {
	// Particle model for compute shaders as an example
	const u32 PARTICLE_COUNT = 2048;

	struct Particle {
		enginemath::Vec4 position;
		enginemath::Vec4 velocity;
		enginemath::Vec4 color;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Particle);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Particle, position);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Particle, color);

			return attributeDescriptions;
		}
	};

	class Storage {
		public:
			Storage(Device& _device, Buffer& _buffer);
			~Storage();
			Storage(const Storage& other) = delete;
			Storage& operator=(const Storage& other) = delete;

			VkBuffer getStorageBuffer(u32 currentFrame) const { return shaderStorageBuffers[currentFrame]; };

		private:
			Device& device;
			Buffer& buffer;

			std::vector<VkBuffer> shaderStorageBuffers;
			std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

			void createStorageBuffer();

			std::vector<Particle> initParticles();
	};
} // namespace Rath