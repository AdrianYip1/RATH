#include "depth.hpp"

// Depth constructor
Rath::Depth::Depth(Device& _device, Swapchain& _swapchain, Image& _image) :
	device(_device), swapchain(_swapchain), image(_image) {
	createDepthResources();
}

// Depth destructor
Rath::Depth::~Depth() {
	vkDestroyImageView(device.getDevice(), depthImageView, nullptr);
	vkDestroyImage(device.getDevice(), depthImage, nullptr);
	vkFreeMemory(device.getDevice(), depthImageMemory, nullptr);
}

// Creates the depthImage and depthImageMemory using the createImage helper
// An image view is also created then the depth image is
// transitioned from UNDEFINED to DEPTH_STENCIL layout
void Rath::Depth::createDepthResources() {
	VkFormat format = findDepthFormat();
	image.createImage(swapchain.getExtent().width, swapchain.getExtent().height, 1, format,
					  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
					  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	image.createImageView(device.getDevice(), depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageView, 1);

	image.transitionImageLayout(depthImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 1);
}

// Function used to find a format that supports tiling optimal and depth stencil bits
// given the input properties
VkFormat Rath::Depth::findDepthFormat() {
	return device.findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
