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

			void drawFrame();

			// can't call vkDeviceWaitIdle(device) in application so have a function here
			void wait();
			

		private:
			Context context;
			Device device;
			Swapchain swapchain;
			Renderpass renderpass;
			Pipeline pipeline;

			VkCommandPool commandPool;
			VkCommandBuffer commandBuffer;

			VkSemaphore imageAvailableSemaphore;
			VkSemaphore renderFinishedSemaphore;
			VkFence inFlightFence;

			void createCommandPool();
			void createCommandBuffer();
			void createSyncObjects();
			void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex);
	};
} // namespace Rath