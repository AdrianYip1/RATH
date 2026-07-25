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

void Rath::Renderer::createCommandBuffer() {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers");
	}
}

void Rath::Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin recording command buffer");
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderpass.getRenderPass();
	renderPassInfo.framebuffer = swapchain.getFramebuffers()[imageIndex];
	renderPassInfo.renderArea.extent = swapchain.getExtent();
	renderPassInfo.renderArea.offset = { 0,0 };

	VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.height = static_cast<u32>(swapchain.getExtent().height);
	viewport.width = static_cast<u32>(swapchain.getExtent().width);
	viewport.maxDepth = 1.0f;
	viewport.minDepth = 0.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = swapchain.getExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	vkCmdDraw(commandBuffer, 3, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer");
	}
}