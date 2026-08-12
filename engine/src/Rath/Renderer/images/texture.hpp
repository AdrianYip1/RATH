// Texture images + mip levels
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
#include <string>

namespace Rath {
	class Texture {
		public:
			Texture(Device& _device, Image& _image, Buffer& _buffer, const std::string texturePath);
			~Texture();
			Texture(const Texture& other) = delete;
			Texture& operator=(const Texture& other) = delete;
		
			// Returns textureImageView
			VkImageView getTextureImageView() const { return textureImageView; };

			// Returns textureSampler
			VkSampler getSampler() const { return textureSampler; };

		private:
			Device& device;
			Image& image;
			Buffer& buffer;

			u32 mipLevels;
			VkImage textureImage;
			VkDeviceMemory textureImageMemory;

			VkImageView textureImageView;
			VkSampler textureSampler;

			// Loads the pixel data with stb_image, stages it into textureImage, and
			// transitions the image to TRANSFER_DST_OPTIMAL then SHADER_READ_ONLY_OPTIMAL
			void createTextureImage(const std::string texturePath);

			// Creates the view used to access textureImage
			void createTextureImageView();

			// Creates the sampler, a reusable set of rules for how any image is sampled
			void createTextureSampler();

			void generateMipmaps(VkImage image, VkFormat imageFormat, u32 texWidth, u32 texHeight, u32 mipLevels);
	};
} // namespace Rath