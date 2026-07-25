#include "renderer.hpp"

Rath::Renderer::Renderer(Window& window) : 
	context(window), 
	device(context), 
	swapchain(window, context, device),
	renderpass(device, swapchain),
	pipeline(device, swapchain, renderpass) {

	createCommandPool();
	std::cout << "Created command pool" << std::endl;
}

Rath::Renderer::~Renderer() {
	vkDestroyCommandPool(device.getDevice(), commandPool, nullptr);
	std::cout << "Destroyed command pool" << std::endl;
}

void Rath::Renderer::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = device.findQueueFamilies(device.getPhysicalDevice());

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device.getDevice(), &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create command pool");
	}
}