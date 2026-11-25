#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "vkDevice.hpp"
#include "vkSwapchain.hpp"
#include "vkPipeline.hpp"

#include "vkBuffer.hpp"

namespace RATH {
	class Render {
	public:
		Render(Device& device, Swapchain& swapchain, Pipeline& pipeline);
		~Render();
		Render(const Render& other) = delete;
		Render& operator=(const Render& other) = delete;

		// Returns true if the swapchain is out of date / suboptimal and must be recreated
		bool drawFrame(Device& device, Swapchain& swapchain, Pipeline& pipeline, 
			const std::vector<std::unique_ptr<Buffer>>& vertexBuffers,
			const std::vector<std::unique_ptr<Buffer>>& indexBuffers);

		// Call after Swapchain::recreate() (image count may change)
		void onSwapchainRecreated(Swapchain& swapchain);
	private:
		VkDevice _device = VK_NULL_HANDLE;
		VkCommandPool _commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSem;
		std::vector<VkSemaphore> renderFinishedSem;
		std::vector<VkFence> inFlightFence;

		u32 currentFrame = 0;

		void createSyncObjects(VkDevice device, size swapchainImageCount);
		void createRenderFinishedSems(VkDevice device, size swapchainImageCount);
		void recordCommandBuffer(Device& device, Swapchain& swapchain, Pipeline& pipeline, 
			const std::vector<std::unique_ptr<Buffer>>& vertexBuffers, 
			const std::vector<std::unique_ptr<Buffer>>& indexBuffers,
			VkCommandBuffer commandBuffer, u32 imageIdx);
		static VkCommandPool createCommandPool(Device& device);
		static std::vector<VkCommandBuffer> createCommandBuffers(Device& device, VkCommandPool commandPool);
	};
} // namespace RATH