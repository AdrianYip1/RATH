#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "vkDevice.hpp"
#include "vkSwapchain.hpp"
#include "vkPipeline.hpp"

#include "vkBuffer.hpp"

#include "vkCheck.hpp"

namespace RATH {
	class Render {
	public:
		Render(Device& device, Swapchain& swapchain, Pipeline& pipeline);
		~Render();
		Render(const Render& other) = delete;
		Render& operator=(const Render& other) = delete;

		// Returns true if the swapchain is out of date / suboptimal and must be recreated
		bool drawFrame(const std::vector<std::unique_ptr<Buffer>>& vertexBuffers,
					   const std::vector<std::unique_ptr<Buffer>>& indexBuffers);

		// Call after Swapchain::recreate() (image count may change)
		void onSwapchainRecreated(Swapchain& swapchain);
	private:
		Device& _device;
		Swapchain& _swapchain;
		Pipeline& _pipeline;

		VkCommandPool _commandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSem;
		std::vector<VkSemaphore> renderFinishedSem;
		std::vector<VkFence> inFlightFence;

		u32 currentFrame = 0;

		void createSyncObjects(size swapchainImageCount);
		void createRenderFinishedSems(size swapchainImageCount);
		void recordCommandBuffer(const std::vector<std::unique_ptr<Buffer>>& vertexBuffers, 
								 const std::vector<std::unique_ptr<Buffer>>& indexBuffers,
								 VkCommandBuffer commandBuffer, u32 imageIdx);
		static VkCommandPool createCommandPool(Device& device);
		static std::vector<VkCommandBuffer> createCommandBuffers(Device& device, VkCommandPool commandPool);
	};
} // namespace RATH