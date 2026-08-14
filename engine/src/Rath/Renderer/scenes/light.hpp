#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/mat4.hpp>
#include <enginemath/vec3.hpp>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "../swapchain.hpp"
#include "../models/model.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <memory>

namespace Rath {
	struct R_LightObject {
		alignas(16) enginemath::Vec3 position = enginemath::Vec3(0.0f);
		alignas(16) enginemath::Vec3 color = enginemath::Vec3(1.0f);
	};

	struct R_LightUbo {
		R_LightObject lights[MAX_LIGHTS];
		int lightCount;
	};

	struct R_SceneLight {
		R_Model* model = nullptr;
		// Location of light
		enginemath::Vec3 position = enginemath::Vec3(0.0f);
		// Colour of light
		enginemath::Vec3 color = enginemath::Vec3(1.0f);
	};
	class R_Light {
		public:
			R_Light(Device& _device, Swapchain& _swapchain,
					Buffer& _buffer);
			~R_Light();
			R_Light(const R_Light& other) = delete;
			R_Light& operator=(const R_Light& other) = delete;

			VkBuffer getLightBuffer(u32 currentFrame) const { return lightBuffers[currentFrame]; };

			void createLightBuffers();
			void updateLights(u32 currentImage, const std::vector<R_SceneLight>& lights);

		private:
			Device& device;
			Swapchain& swapchain;
			Buffer& buffer;

			std::vector<VkBuffer> lightBuffers;
			std::vector<VkDeviceMemory> lightBuffersMemory;
			std::vector<void*> lightBuffersMapped;
	};
} // namespace Rath