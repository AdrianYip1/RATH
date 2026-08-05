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

			// Creates the image and its memory, storing them in the passed references,
			// then binds the memory to the image
			void createImage(u32 width, u32 height, u32 mipLevels, VkSampleCountFlagBits numSamples, 
							 VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
							 VkMemoryPropertyFlags properties, VkImage& image,
							 VkDeviceMemory& imageMemory);

			// Transitions an image's layout using a pipeline barrier,
			// format is only used to check whether the image has a stencil component
			void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, 
									   VkImageLayout newLayout, u32 mipLevels);

			// Copies the buffer's contents into an image
			void copyBufferToImage(VkBuffer _buffer, VkImage image, u32 width, u32 height);

			// Creates an image view, a specific way of accessing the provided image,
			// stored in the imageView reference
			static void createImageView(VkDevice device, VkImage image, VkFormat format, 
										VkImageAspectFlags aspectFlags, VkImageView& imageView,
										u32 mipLevels);
		
		private:
			Device& device;
			Buffer& buffer;	
	};
} // namespace Rath