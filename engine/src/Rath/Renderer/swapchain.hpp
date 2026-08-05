#pragma once
#include <GLFW/glfw3.h>
#include "vulkan/vulkan.h"

// Rath files
#include "Rath/Core/defines.hpp"
#include "device.hpp"
#include "context.hpp"
#include "images/image.hpp"
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

			// Returns swapChain
			VkSwapchainKHR getSwapchain() const { return swapChain; };

			// Returns the number of images the swapchain owns
			u32 getImageCount() const { return swapChainImages.size(); };

			// Returns swapChainExtent
			VkExtent2D getExtent() const { return swapChainExtent; };

			// Returns swapChainImageFormat
			VkFormat getFormat() const { return swapChainImageFormat; };

			// Returns swapChainFramebuffers
			const std::vector<VkFramebuffer>& getFramebuffers() const { return swapChainFramebuffers; };

			// Creates a framebuffer per swapchain image, each holding that image's
			// view and the depth view. Call this after the graphics pipeline is created
			void createFramebuffers(VkRenderPass renderpass, VkImageView depthView, VkImageView colorView);

			// On resize or minimize, waits for the window to have a size then rebuilds
			// the swapchain, image views and framebuffers
			void recreateSwapChain(VkRenderPass renderpass, VkImageView depthView);

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

			// Creates the swapchain from the queried support details and the queue
			// families, then populates swapChainImages with the images it already created
			void createSwapChain();

			// Creates an image view per swapchain image, a way to interpret each one
			void createImageViews();

			// Destroys the framebuffers, image views and swapchain, called on destruction
			// and before every recreation
			void cleanupSwapChain();

			// Returns B8G8R8A8_SRGB with a nonlinear sRGB color space if available,
			// otherwise the first available format
			VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

			// Returns MAILBOX if available, otherwise FIFO which is always supported
			VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

			// Returns the surface's currentExtent when it is set, otherwise the window's
			// framebuffer size clamped to the surface's allowed range
			VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	};
} // namespace Rath