#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "modelStruct.hpp"
#include "material.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>


namespace Rath {
	class Model {
		public:
			Model() = default;
			~Model();
			Model(const Model& other) = delete;
			Model& operator=(const Model& other) = delete;

			static bool rCreateModel(Device& _device, Buffer& _buffer,
									 const R_ModelCreateInfo& info, Model* _model);

			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);
			void bindDescriptors(VkCommandBuffer commandBuffer, VkPipelineLayout layout);


		private:
			Device* device = nullptr;
			Buffer* buffer = nullptr;

			// Model that is being loaded
			std::string modelPath;

			std::vector<Vertex> vertices;
			std::vector<u32> indices;

			VkBuffer vertexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
			VkBuffer indexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

			// Model stores the material
			R_Material* material = nullptr;

			// Uses a staging buffer to copy vertices into the device local vertexBuffer,
			// staging buffer is destroyed after
			void createVertexBuffer();

			// Uses a staging buffer to copy indices into the device local indexBuffer,
			// staging buffer is destroyed after
			void createIndexBuffer();

			void loadModel();

	};
} // namespace Rath