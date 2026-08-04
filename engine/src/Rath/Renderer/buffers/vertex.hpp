#pragma once
#include <vulkan/vulkan.h>
#include <enginemath/vec2.hpp>
#include <enginemath/vec3.hpp>

// Rath files
#include "../vertexData.hpp"
#include "Rath/Core/defines.hpp"
#include "../device.hpp"
#include "buffer.hpp"
#include "../models/model.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {

	class VertexBuffer {
		public:
			VertexBuffer(Device& _device, Buffer& _buffer, Model& _model);
			~VertexBuffer();
			VertexBuffer(const VertexBuffer& other) = delete;
			VertexBuffer& operator=(const VertexBuffer& other) = delete;

			// Returns vertexBuffer
			VkBuffer getVertexBuffer() const { return vertexBuffer; };

			// Returns indexBuffer
			VkBuffer getIndexBuffer() const { return indexBuffer; };

			// Vertex and indices for models
			std::vector<Vertex> vertices;
			std::vector<u32> indices;

		private:
			Device& device;
			Buffer& buffer;
			Model& model;

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
	};
} // namespace Rath