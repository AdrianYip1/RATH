#include "swapchain.hpp"

RATH::Swapchain::Swapchain(Context& _context, Device& _device) :
	context(_context), device(_device) {
	
}

RATH::Swapchain::~Swapchain() {

}

VkPresentModeKHR RATH::Swapchain::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkSurfaceFormatKHR RATH::Swapchain::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkExtent2D RATH::Swapchain::chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities) {

}
