#include "color.hpp"

// Depth constructor
Rath::Color::Color(Device& _device, Swapchain& _swapchain, Image& _image) :
	device(_device), swapchain(_swapchain), image(_image) {
	createColorResources();
}

// Depth destructor
Rath::Color::~Color() {
	vkDestroyImageView(device.getDevice(), colorImageView, nullptr);
	vkDestroyImage(device.getDevice(), colorImage, nullptr);
	vkFreeMemory(device.getDevice(), colorImageMemory, nullptr);
}

void Rath::Color::createColorResources() {
	VkFormat colorFormat = swapchain.getFormat();

	image.createImage(swapchain.getExtent().width, swapchain.getExtent().height, 1, device.getMSAASampleCount(),
					  colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT |
					  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					  colorImage, colorImageMemory);
	image.createImageView(device.getDevice(), colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, colorImageView, 1);
}