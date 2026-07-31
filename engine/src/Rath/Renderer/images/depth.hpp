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

			VkFormat findDepthFormat();
			VkImageView getDepthImageView() { return depthImageView; };
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