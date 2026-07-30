#pragma once
#include <vulkan/vulkan.h>


// Rath files
#include "Rath/Core/defines.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "image.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Texture {
		public:
			Texture(Device& _device, Image& _image, Buffer& _buffer);
			~Texture();
			Texture(const Texture& other) = delete;
			Texture& operator=(const Texture& other) = delete;
		
			VkImageView getTextureImageView() { return textureImageView; };
			VkSampler getSampler() { return textureSampler; };

		private:
			Device& device;
			Image& image;
			Buffer& buffer;

			VkImage textureImage;
			VkDeviceMemory textureImageMemory;

			VkImageView textureImageView;
			VkSampler textureSampler;

			void createTextureImage();
			void createTextureImageView();
			void createTextureSampler();

	};
} // namespace Rath