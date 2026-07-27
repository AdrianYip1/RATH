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
#include "Rath/Platform/window.hpp"

// std
#include <stdexcept>


namespace Rath {
	// The amount of frames that can be processed at the same time
	const int MAX_FRAMES_IN_FLIGHT = 2;

	class Renderer {
		public:

			Renderer(Window& window);
			~Renderer();
			Renderer(const Renderer& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

			void drawFrame();

			// can't call vkDeviceWaitIdle(device) in application so have a function here
			void wait();

		private:
			Window& window;
			Context context;
			Device device;
			Swapchain swapchain;
			Renderpass renderpass;
			Pipeline pipeline;
			VertexBuffer vertexBuffer;

			VkCommandPool commandPool;
			std::vector<VkCommandBuffer> commandBuffers;

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;

			u32 currentFrame = 0;

			void createCommandPool();
			void createCommandBuffers();
			void createSyncObjects();
			void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex);
	};
} // namespace Rath