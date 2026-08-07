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
		enginemath::Vec2 position;
		enginemath::Vec2 velocity;
		enginemath::Vec4 color;
	};

	class Storage {
		public:
			Storage(Device& _device, Buffer& _buffer);
			~Storage();
			Storage(const Storage& other) = delete;
			Storage& operator=(const Storage& other) = delete;

			VkBuffer getStorageBuffer(u32 currentFrame) { return shaderStorageBuffers[currentFrame]; };

		private:
			Device& device;
			Buffer& buffer;

			std::vector<VkBuffer> shaderStorageBuffers;
			std::vector<VkDeviceMemory> shaderStorageBuffersMemory;

			void createStorageBuffer();

			std::vector<Particle> initParticles();
	};
} // namespace Rath