#pragma once
#include <vulkan/vulkan.h>

#include <Rath/Core/defines.hpp>
#include "../Platform/window.hpp"
#include "context.hpp"
#include "device.hpp"

namespace RATH {
	class Swapchain {
	public:
		Swapchain(Window& _window, Context& _context, Device& _device);
		~Swapchain();
		Swapchain(const Swapchain& other) = delete;
		Swapchain& operator=(const Swapchain& other) = delete;

	private:
		Window& window;
		Context& context;
		Device& device;

		VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createSwapchain();

	};
} // namespace RATH