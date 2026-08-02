#include "texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

// Texture constructor
Rath::Texture::Texture(Device& _device, Image& _image, Buffer& _buffer) :
	device(_device), image(_image), buffer(_buffer) {
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
}

// Texture destructor
Rath::Texture::~Texture() {
	vkDestroySampler(device.getDevice(), textureSampler, nullptr);
	vkDestroyImageView(device.getDevice(), textureImageView, nullptr);
	vkDestroyImage(device.getDevice(), textureImage, nullptr);
	vkFreeMemory(device.getDevice(), textureImageMemory, nullptr);
}

// Makes use of the stb_image lib to obtain the pixel data of a texture image, w, h and # channels
// A staging buffer is used to get the pixel information, then textureImage is created
// and transitioned to TRANSFER_DST_OPTIMAL where copyBufferToImage is called.
// After the pixel data from the staging buffer is copied, the textureImage is transitioned 
// again to LAYOUT_SHADER_READ_ONLY_OPTIMAL
// Staging buffer + memory are destroyed and freed at the end
void Rath::Texture::createTextureImage() {
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("../../../../engine/textures/texture.jpg", &texWidth,
		&texHeight, &texChannels, STBI_rgb_alpha);
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image");
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	buffer.createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
						VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
						stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device.getDevice(), stagingBufferMemory, 0, imageSize, 0, &data);
	memcpy(data, pixels, static_cast<size>(imageSize));
	vkUnmapMemory(device.getDevice(), stagingBufferMemory);

	stbi_image_free(pixels);

	image.createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
					  VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

	image.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	image.copyBufferToImage(stagingBuffer, textureImage, static_cast<u32>(texWidth), static_cast<u32>(texHeight));
	image.transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
								VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device.getDevice(), stagingBufferMemory, nullptr);
}

// Helper function that creates the texture image view
void Rath::Texture::createTextureImageView() {
	image.createImageView(device.getDevice(), textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, textureImageView);
}

// Creates the texture sampler, basically a set of rules that is used for any textureImage
// which specifies how to sample the image
void Rath::Texture::createTextureSampler() {
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties(device.getPhysicalDevice(), &properties);
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;

	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device.getDevice(), &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler");
	}
}