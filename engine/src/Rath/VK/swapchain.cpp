#include "swapchain.hpp"

RATH::Swapchain::Swapchain(Window& _window, Context& _context, Device& _device) :
	window(_window), context(_context), device(_device) {

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
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    }
    else {
        int width, height;
        glfwGetFramebufferSize(window.getWindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void RATH::Swapchain::createSwapchain() {
    SwapchainSupportDetails swapchainSupport = device.querySwapchainSupport(device.getPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(swapchainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapchainPresentMode(swapchainSupport.presentModes);
    VkExtent2D extent = chooseSwapchainExtent(swapchainSupport.capabilities);
}
