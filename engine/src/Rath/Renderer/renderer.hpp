// Contains frames in flight sync, command buffers,
// pools/sets, draw loop
#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "context.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "pipeline.hpp"
#include "buffers/vertex.hpp"
#include "buffers/buffer.hpp"
#include "buffers/uniform.hpp"
#include "images/image.hpp"
#include "descriptor.hpp"
#include "images/texture.hpp"
#include "images/color.hpp"
#include "buffers/storage.hpp"
#include "images/depth.hpp"
#include "models/model.hpp"
#include "Rath/Platform/window.hpp"

// std
#include <stdexcept>


namespace Rath {
	class Renderer {
		public:

			Renderer(Window& window);
			~Renderer();
			Renderer(const Renderer& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;
			
			// Draws one frame: waits on the fence, acquires an image, updates the
			// uniform buffer, records and submits, then presents
			void drawFrame();

			// Waits for all queues to finish, needed before destruction
			// can't call vkDeviceWaitIdle(device) in application so have a function here
			void wait();

		private:
			Window& window;
			Context context;
			Device device;
			Swapchain swapchain;
			Buffer buffer;
			Model model;
			VertexBuffer vertexBuffer;
			Image image;
			Color color;
			Depth depth;
			Renderpass renderpass;
			Texture texture;
			UniformBuffer uniformBuffer;
			Storage storage;
			Descriptor descriptor;
			Pipeline pipeline;
			

			std::vector<VkCommandBuffer> commandBuffers;
			std::vector<VkCommandBuffer> computeCommandBuffers;

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;
			std::vector<VkSemaphore> computeFinishedSemaphores;
			std::vector<VkFence> computeInFlightFences;

			u32 currentFrame = 0;

			// Allocates a command buffer per frame in flight from the command pool,
			// freed when the command pool is destroyed
			void createCommandBuffers();

			// Creates the semaphores and fences needed for rendering
			void createSyncObjects();

			// Begins recording, starts the render pass, binds the pipeline, sets the
			// dynamic viewport and scissor, then binds the vertex, descriptor and
			// index data before drawing
			void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex);
			
			void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);
	};
} // namespace Rath