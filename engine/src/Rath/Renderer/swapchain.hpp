#pragma once
#include <GLFW/glfw3.h>
#include "vulkan/vulkan.h"

// Rath files
#include "Rath/Core/defines.hpp"
#include "device.hpp"
#include "context.hpp"
#include "Rath/Platform/window.hpp"

// std
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint> 
#include <limits> 
#include <algorithm> 


namespace Rath {
	class Swapchain {
		public:
			Swapchain(Window& _window, Context& _context, Device& _device);
			~Swapchain();
			Swapchain(const Swapchain& other) = delete;
			Swapchain& operator=(const Swapchain& other) = delete;

			VkSwapchainKHR getSwapchain();
			u32 getImageCount();
			VkExtent2D getExtent();
			VkFormat getFormat();
			std::vector<VkFramebuffer> getFramebuffers();
			// Call this function after graphics pipeline is created
			void createFramebuffers(VkRenderPass renderpass);

		private:
			Window& window;
			Context& context;
			Device& device;

			VkSwapchainKHR swapChain;
			std::vector<VkImage> swapChainImages;
			/* examples for swapChainImages + usage
			swapChainImages[0] == 0x000001A2B4C05080   // a handle: a pointer to image A
			swapChainImages[1] == 0x000001A2B4C05100   // a handle: a pointer to image B
			swapChainImages[2] == 0x000001A2B4C05180   // a handle: a pointer to image C
			*/
			VkFormat swapChainImageFormat;
			VkExtent2D swapChainExtent;
			std::vector<VkImageView> swapChainImageViews;
			std::vector<VkFramebuffer> swapChainFramebuffers;

			void createSwapChain();
			void createImageViews();

			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
} // namespace Rath