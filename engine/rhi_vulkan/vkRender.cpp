#include "vkRender.hpp"

RATH::Render::Render(Device& device, Swapchain& swapchain, Pipeline& pipeline) :
	_device(device), _swapchain(swapchain), _pipeline(pipeline),
	_commandPool(createCommandPool(device)), 
	commandBuffers(createCommandBuffers(device, _commandPool))
{
	createSyncObjects(swapchain.getImages().size());
};

RATH::Render::~Render() {
	for (VkSemaphore sem : imageAvailableSem) vkDestroySemaphore(_device.getDevice(), sem, nullptr);
	for (VkSemaphore sem : renderFinishedSem) vkDestroySemaphore(_device.getDevice(), sem, nullptr);
	for (VkFence fence : inFlightFence) vkDestroyFence(_device.getDevice(), fence, nullptr);

	vkDestroyCommandPool(_device.getDevice(), _commandPool, nullptr);
};

// CPU -> recording command buffer
// GPU -> submitting/executing command buffer
bool RATH::Render::drawFrame(const std::vector<std::unique_ptr<Buffer>>& vertexBuffers,
							 const std::vector<std::unique_ptr<Buffer>>& indexBuffers) {
	vkWaitForFences(_device.getDevice(), 1, &inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

	u32 imageIdx;
	VkResult acquireResult = vkAcquireNextImageKHR(_device.getDevice(), _swapchain.getSwapchain(), UINT64_MAX, imageAvailableSem[currentFrame], VK_NULL_HANDLE, &imageIdx);
	if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
		return true;
	}
	if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("ERROR: vkRender.cpp -> drawFrame() acquire");
	}

	// Reset only once we know we will submit, otherwise an early return leaves it unsignaled -> deadlock
	vkResetFences(_device.getDevice(), 1, &inFlightFence[currentFrame]);

	VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
	vkResetCommandBuffer(commandBuffer, 0);
	recordCommandBuffer(vertexBuffers, indexBuffers, commandBuffer, imageIdx);

	// Submit info
	VkSemaphore waitSemaphore[] = { imageAvailableSem[currentFrame] };
	VkPipelineStageFlags waitStage[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

	VkSemaphore signalSemaphore[] = { renderFinishedSem[imageIdx] };

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = waitSemaphore,
		.pWaitDstStageMask = waitStage,

		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer,

		.signalSemaphoreCount = 1,
		.pSignalSemaphores = signalSemaphore,
	};

	// Fence must be unsignaled, same with signalSemaphores
	if (vkQueueSubmit(_device.getGraphicsQ(), 1, &submitInfo, inFlightFence[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkRender.cpp -> drawFrame()");
	}

	// Present queue
	VkSwapchainKHR swapchains[] = { _swapchain.getSwapchain()};

	VkPresentInfoKHR presentInfo = {
		.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,

		.waitSemaphoreCount = 1,
		.pWaitSemaphores = signalSemaphore,

		.swapchainCount = 1,
		.pSwapchains = swapchains,

		.pImageIndices = &imageIdx,
	};

	// vkQueuePresentKHR doesn't wait, returns right away
	VkResult presentResult = vkQueuePresentKHR(_device.getPresentQ(), &presentInfo);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

	if (presentResult == VK_ERROR_OUT_OF_DATE_KHR || presentResult == VK_SUBOPTIMAL_KHR) {
		return true;
	}
	if (presentResult != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkRender.cpp -> drawFrame() present");
	}
	return false;
}

void RATH::Render::onSwapchainRecreated(Swapchain& swapchain) {
	for (VkSemaphore sem : renderFinishedSem) vkDestroySemaphore(_device.getDevice(), sem, nullptr);
	renderFinishedSem.clear();
	createRenderFinishedSems(swapchain.getImages().size());
}

void RATH::Render::createSyncObjects(size swapchainImageCount) {
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fenceCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	// Per frame in flight: CPU frame pacing
	imageAvailableSem.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(_device.getDevice(), &semaphoreCreateInfo, nullptr, &imageAvailableSem[i]) != VK_SUCCESS ||
			vkCreateFence(_device.getDevice(), &fenceCreateInfo, nullptr, &inFlightFence[i]) != VK_SUCCESS) {
			throw std::runtime_error("ERROR: vkRender.cpp -> createSyncObjects()");
		}
	}

	createRenderFinishedSems(swapchainImageCount);
}

void RATH::Render::createRenderFinishedSems(size swapchainImageCount) {
	VkSemaphoreCreateInfo semaphoreCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	// Per swapchain image: presentation engine holds it until that image is reacquired
	renderFinishedSem.resize(swapchainImageCount);

	for (size i = 0; i < swapchainImageCount; i++) {
		if (vkCreateSemaphore(_device.getDevice(), &semaphoreCreateInfo, nullptr, &renderFinishedSem[i]) != VK_SUCCESS) {
			throw std::runtime_error("ERROR: vkRender.cpp -> createSyncObjects()");
		}
	}
}

void RATH::Render::recordCommandBuffer(const std::vector<std::unique_ptr<Buffer>>& vertexBuffers,
									   const std::vector<std::unique_ptr<Buffer>>& indexBuffers,
									   VkCommandBuffer commandBuffer, u32 imageIdx) {
	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
	};

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkRender.cpp -> recordCommandBuffer()");
	}

	// Barrier 1 -> before rendering
	VkImageMemoryBarrier2 toColor = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = 0,
		.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,

		.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = _swapchain.getImages()[imageIdx],
		.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
	};

	VkDependencyInfo toColorDep = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &toColor,
	};
	vkCmdPipelineBarrier2(commandBuffer, &toColorDep);

	// Begin rendering (instead of renderpass + framebuffer)
	VkRenderingAttachmentInfo colorAttachment = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
		.imageView = _swapchain.getImageViews()[imageIdx],
		.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
		.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		.clearValue = {
			.color = {
				.float32 = {0.1f, 0.1f, 0.1f, 1.0f}
			}
		}
	};

	VkRenderingInfo rendering = {
		.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
		.renderArea = {
			.offset = {0, 0},
			.extent = _swapchain.getExtent()
		},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachment,
	};
	vkCmdBeginRendering(commandBuffer, &rendering);

	// Bind pipeline/viewport/scissor
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline.getPipeline());

	VkViewport viewport = {
		.x = 0.0f,
		.y = 0.0f,
		.width = static_cast<f32>(_swapchain.getExtent().width),
		.height = static_cast<f32>(_swapchain.getExtent().height),
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor = {
		.offset = {0, 0},
		.extent = _swapchain.getExtent(),
	};
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	for (size i = 0; i < vertexBuffers.size(); i++) {
		VkBuffer buffers[] = { vertexBuffers[i]->getBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, indexBuffers[i]->getBuffer(), 0, VK_INDEX_TYPE_UINT16);

		vkCmdDrawIndexed(commandBuffer, static_cast<u32>(indexBuffers[i]->getSize() / sizeof(u16)), 1, 0, 0, 0);
	}

	vkCmdEndRendering(commandBuffer);

	// Barrier 2 -> after rendering
	VkImageMemoryBarrier2 toPresent = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
		.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		.dstStageMask = VK_PIPELINE_STAGE_2_NONE,

		.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = _swapchain.getImages()[imageIdx],
		.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1},
	};

	VkDependencyInfo toPresentDep = {
		.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &toPresent,
	};
	vkCmdPipelineBarrier2(commandBuffer, &toPresentDep);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkRender.cpp -> recordCommandBuffer()");
	}
}

VkCommandPool RATH::Render::createCommandPool(Device& device) {
	std::optional<QueueInfos> QFamilyIdx = device.findQueueFamilies(device.getPhysicalDevice(), device.getSurface());

	VkCommandPoolCreateInfo poolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
		.queueFamilyIndex = QFamilyIdx.value().graphicsQueueIdx.value()
	};

	VkCommandPool commandPool = VK_NULL_HANDLE;

	if (vkCreateCommandPool(device.getDevice(), &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkRender.cpp -> createCommandPool()");
	}

	return commandPool;
}

std::vector<VkCommandBuffer> RATH::Render::createCommandBuffers(Device& device, VkCommandPool commandPool) {
	std::vector<VkCommandBuffer> commandBuffers(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo commandBufferAllocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = commandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = static_cast<u32>(commandBuffers.size())
	};

	if (vkAllocateCommandBuffers(device.getDevice(), &commandBufferAllocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkRender.cpp -> createCommandBuffers()");
	}

	return commandBuffers;
}