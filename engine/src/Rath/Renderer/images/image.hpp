#pragma once
#include <vulkan/vulkan.h>


// Rath files
#include "Rath/Core/defines.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"


// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Image {
		public:
			Image(Device& _device, Buffer& _buffer);
			~Image();
			Image(const Image& other) = delete;
			Image& operator=(const Image& other) = delete;

			void createImage(u32 width, u32 height, VkFormat format,
							 VkImageTiling tiling, VkImageUsageFlags usage,
							 VkMemoryPropertyFlags properties, VkImage& image,
							 VkDeviceMemory& imageMemory);
		
			VkImageView getTextureImageView();
			VkSampler getSampler();
		private:
			Device& device;
			Buffer& buffer;

			VkImage textureImage;
			VkDeviceMemory textureImageMemory;

			VkImageView textureImageView;
			VkSampler textureSampler;

			void createTextureImage();
			void transitionImageLayout(VkImage image, VkFormat format,
									   VkImageLayout oldLayout, VkImageLayout newLayout);
			void copyBufferToImage(VkBuffer _buffer, VkImage image, u32 width, u32 height);
			void createTextureImageView();
			void createTextureSampler();
	
	};
} // namespace Rath