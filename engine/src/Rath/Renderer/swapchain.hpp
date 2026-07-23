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
		private:
			Window& window;
			Context& context;
			Device& device;

			VkSwapchainKHR swapChain;
			std::vector<VkImage> swapChainImages;
			VkFormat swapChainImageFormat;
			VkExtent2D swapChainExtent;

			void createSwapChain();
			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
			VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
} // namespace Rath