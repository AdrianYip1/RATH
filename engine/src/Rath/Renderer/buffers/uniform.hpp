#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/mathutils.hpp>
#include <enginemath/vec2.hpp>
#include <enginemath/vec3.hpp>
#include <enginemath/mat4.hpp>

// Rath files
#include "Rath/Core/defines.hpp"
#include "Rath/Core/camera/camera.hpp"
#include "../device.hpp"
#include "../swapchain.hpp"
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
		enginemath::Mat4 view;
		enginemath::Mat4 proj;
		f32 uTime;
	};

	class UniformBuffer {
		public:
			UniformBuffer(Device& _device, Swapchain& _swapchain, 
						  Buffer& _buffer, Camera& _camera);
			~UniformBuffer();
			UniformBuffer(const UniformBuffer& other) = delete;
			UniformBuffer& operator=(const UniformBuffer& other) = delete;
	
			// Changes the values in the ubo struct and copies them into
			// uniformBuffersMapped at the current frame
			void updateUniformBuffer(u32 currentImage);

			// Returns uniformBuffers at currentFrame
			VkBuffer getUniformBuffer(u32 currentFrame) const { return uniformBuffers[currentFrame]; };
		
		private:
			Device& device;
			Swapchain& swapchain;
			Buffer& buffer;
			Camera& camera;

			// New data is copied into uniform buffer every frame
			// no point in having staging buffer
			// Should use multiple buffers as multiple frames
			// may be in flight -> dont want to overwrite buffers 
			std::vector<VkBuffer> uniformBuffers;
			std::vector<VkDeviceMemory> uniformBuffersMemory;
			std::vector<void*> uniformBuffersMapped;

			// Creates a uniform buffer and memory per frame in flight, then
			// persistently maps them into uniformBuffersMapped
			void createUniformBuffers();

	};

} // namespace Rath