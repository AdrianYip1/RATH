#include "depth.hpp"

Rath::Depth::Depth(Device& _device, Swapchain& _swapchain, Image& _image) :
	device(_device), swapchain(_swapchain), image(_image) {
	createDepthResources();
}

Rath::Depth::~Depth() {
	vkDestroyImageView(device.getDevice(), depthImageView, nullptr);
	vkDestroyImage(device.getDevice(), depthImage, nullptr);
	vkFreeMemory(device.getDevice(), depthImageMemory, nullptr);

}

void Rath::Depth::createDepthResources() {
	VkFormat format = findDepthFormat();
	image.createImage(swapchain.getExtent().width, swapchain.getExtent().height, format,
					  VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 
					  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	image.createImageView(device.getDevice(), depthImage, format, VK_IMAGE_ASPECT_DEPTH_BIT, depthImageView);

	image.transitionImageLayout(depthImage, format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

VkFormat Rath::Depth::findDepthFormat() {
	return device.findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
