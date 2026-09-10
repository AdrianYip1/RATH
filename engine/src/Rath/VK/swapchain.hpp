#pragma once
#include <vulkan/vulkan.h>

#include <Rath/Core/defines.hpp>
#include "context.hpp"
#include "device.hpp"

namespace RATH {
	class Swapchain {
	public:
		Swapchain(Context& _context, Device& _device);
		~Swapchain();
		Swapchain(const Swapchain& other) = delete;
		Swapchain& operator=(const Swapchain& other) = delete;

	private:
		Context& context;
		Device& device;

		VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities);
		void createSwapchain();

	};
} // namespace RATH