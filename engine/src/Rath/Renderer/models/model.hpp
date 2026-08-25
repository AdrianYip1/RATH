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

struct cgltf_node;

namespace Rath {
	class R_Model {
		public:
			R_Model() = default;
			~R_Model();
			R_Model(const R_Model& other) = delete;
			R_Model& operator=(const R_Model& other) = delete;

			void handleNodes(cgltf_node* node);
			static bool rCreateModel(Device& _device, Buffer& _buffer,
									 const R_ModelCreateInfo& info, R_Model* _model);

			void bind(VkCommandBuffer commandBuffer);
			void draw(VkCommandBuffer commandBuffer);
			void bindDescriptors(VkCommandBuffer commandBuffer);
			void bindPipeline(VkCommandBuffer commandBuffer);

			VkPipeline getPipeline() const { return pipeline; };
			VkPipelineLayout getPipelineLayout() const { return pipelineLayout; };
			std::vector<R_Primitive>& getPrimitives() { return primitives; };

		private:
			Device* device = nullptr;
			Buffer* buffer = nullptr;

			// Model that is being loaded
			std::string modelPath;

			std::vector<Vertex> vertices;
			std::vector<u32> indices;
			std::vector<R_Primitive> primitives;

			VkBuffer vertexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory vertexBufferMemory = VK_NULL_HANDLE;
			VkBuffer indexBuffer = VK_NULL_HANDLE;
			VkDeviceMemory indexBufferMemory = VK_NULL_HANDLE;

			//Pipeline* pipeline = nullptr;
			VkPipeline pipeline = nullptr;
			VkPipelineLayout pipelineLayout = nullptr;

			// Uses a staging buffer to copy vertices into the device local vertexBuffer,
			// staging buffer is destroyed after
			void createVertexBuffer();

			// Uses a staging buffer to copy indices into the device local indexBuffer,
			// staging buffer is destroyed after
			void createIndexBuffer();

			void loadModel();

	};
} // namespace Rath