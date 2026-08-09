#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <unordered_map>

namespace Rath {
	class Model {
		public:
			Model(Device& _device, Buffer& _buffer, const std::string& path);
			~Model();
			Model(const Model& other) = delete;
			Model& operator=(const Model& other) = delete;

			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);

		private:
			Device& device;
			Buffer& buffer;
			// Model that is being loaded
			const std::string modelPath;

			std::unordered_map<Vertex, u32> uniqueVertices{};
			std::vector<Vertex> vertices;
			std::vector<u32> indices;

			VkBuffer vertexBuffer;
			VkDeviceMemory vertexBufferMemory;
			VkBuffer indexBuffer;
			VkDeviceMemory indexBufferMemory;

			// Uses a staging buffer to copy vertices into the device local vertexBuffer,
			// staging buffer is destroyed after
			void createVertexBuffer();

			// Uses a staging buffer to copy indices into the device local indexBuffer,
			// staging buffer is destroyed after
			void createIndexBuffer();

			void loadModel();
	};
} // namespace Rath