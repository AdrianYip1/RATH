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
	class Color {
	public:
		Color(Device& _device, Swapchain& _swapchain, Image& _image);
		~Color();
		Color(const Color& other) = delete;
		Color& operator=(const Color& other) = delete;

		VkImage getColorImage() { return colorImage; };
		VkImageView getColorImageView() { return colorImageView; };

		void createColorResources();

	private:
		Device& device;
		Swapchain& swapchain;
		Image& image;

		VkImage colorImage;
		VkDeviceMemory colorImageMemory;
		VkImageView colorImageView;
	};
} // namespace Rath