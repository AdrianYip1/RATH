#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "image.hpp"
#include "../swapchain.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Depth {
		public:
			Depth(Device& _device, Swapchain& _swapchain, Image& _image);
			~Depth();
			Depth(const Depth& other) = delete;
			Depth& operator=(const Depth& other) = delete;

			// Returns a format supporting optimal tiling and the depth stencil attachment bit
			VkFormat findDepthFormat();

			// Returns depthImageView
			VkImageView getDepthImageView() const { return depthImageView; };

			// Creates the depth image, its memory and its view, then transitions
			// the image from UNDEFINED to DEPTH_STENCIL_ATTACHMENT_OPTIMAL
			void createDepthResources();

		private:
			Device& device;
			Swapchain& swapchain;
			Image& image;

			VkImage depthImage;
			VkDeviceMemory depthImageMemory;
			VkImageView depthImageView;
	};
} // namespace Rath