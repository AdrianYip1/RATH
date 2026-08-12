#include "renderer.hpp"

// Renderer constructor
Rath::Renderer::Renderer(Window& _window, Camera& _camera, Context& _context, 
						 Device& _device, Buffer& _buffer) :
	window(_window),
	camera(_camera),
	context(_context),
	device(_device),
	buffer(_buffer),
	swapchain(_window, context, device),
	image(device, buffer),
	color(device, swapchain, image),
	depth(device, swapchain, image),
	renderpass(device, swapchain, depth),
	uniformBuffer(device, swapchain, buffer, camera),
	storage(device, buffer),
	descriptor(device, uniformBuffer, storage),
	pipeline(device, swapchain, renderpass, descriptor) {

	swapchain.createFramebuffers(renderpass.getRenderPass(), depth.getDepthImageView(), color.getColorImageView());
	camera.setAspect(swapchain.getExtent().width / (f32) swapchain.getExtent().height);
	createCommandBuffers();
	createSyncObjects();
	setUpModel(MODEL_PATH, TEXTURE_PATH, &rMaterial, &rModel);
	setUpModel(MODEL2_PATH, TEXTURE2_PATH, &rCupMaterial, &cupModel);
}

// Renderer destructor
Rath::Renderer::~Renderer() {
	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device.getDevice(), imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device.getDevice(), inFlightFences[i], nullptr);
		vkDestroySemaphore(device.getDevice(), computeFinishedSemaphores[i], nullptr);
		vkDestroyFence(device.getDevice(), computeInFlightFences[i], nullptr);
	}
	for (size i = 0; i < swapchain.getImageCount(); i++) {
		vkDestroySemaphore(device.getDevice(), renderFinishedSemaphores[i], nullptr);
	}
}

// NOTE: renderFinishedSemaphore is resized to the number of images the swapchain owns (swapchain.getImageCount())
// renderFinishedSemaphore belongs to the presentation of a specific image, not the frame of each loop
// so any of the N images need a semaphore to prevent collisions
// THE ISSUE BEFORE:
// vkQueueSubmit for frame 2 might use signalSemaphore[0] thats already signaled (illegal)
// this might happen IF present hasnt consumed + reset the semaphore. There isnt a way to check
// if signalSemaphore[0] is signaled or not.
// 
// having imageIndex size makes it so sem[0] is only reached when aquire returns image 0,
// the driver can't return image 0 while image 0's present hasnt consumed yet
void Rath::Renderer::drawFrame() {
	vkWaitForFences(device.getDevice(), 1, &computeInFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uniformBuffer.updateUniformBuffer(currentFrame);

	// Reset fences only when submitting work
	vkResetFences(device.getDevice(), 1, &computeInFlightFences[currentFrame]);

	// vkResetCommandBuffer is redundant since the pool is created with 
	// VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	// vkResetCommandBuffer(computeCommandBuffers[currentFrame], 0);
	recordComputeCommandBuffer(computeCommandBuffers[currentFrame]);

	VkSubmitInfo computeSubmitInfo{};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.pCommandBuffers = &computeCommandBuffers[currentFrame];
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pSignalSemaphores = &computeFinishedSemaphores[currentFrame];
	computeSubmitInfo.signalSemaphoreCount = 1;

	// Both the semaphore and fence signal when the GPU finished executing every
	// command in the submit
	if (vkQueueSubmit(device.getComputeQueue(), 1, &computeSubmitInfo, computeInFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit to compute queue");
	}

	vkWaitForFences(device.getDevice(), 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	u32 imageIndex;
	VkResult result = vkAcquireNextImageKHR(device.getDevice(), swapchain.getSwapchain(), UINT64_MAX,
		imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		swapchain.recreateSwapChain(renderpass.getRenderPass(), depth.getDepthImageView());
		color.createColorResources();
		depth.createDepthResources();
		camera.setAspect(swapchain.getExtent().width / (f32) swapchain.getExtent().height);
		swapchain.createFramebuffers(renderpass.getRenderPass(), depth.getDepthImageView(), color.getColorImageView());
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("Failed to aquire swapchain image");
	}

	vkResetFences(device.getDevice(), 1, &inFlightFences[currentFrame]);

	// vkResetCommandBuffer is redundant since the pool is created with 
	// VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT
	// vkResetCommandBuffer(commandBuffers[currentFrame], 0);

	recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { computeFinishedSemaphores[currentFrame], imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 2;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[imageIndex] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("Failed to submit draw command buffer");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapchain.getSwapchain() };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(device.getPresentQueue(), &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.getFramebufferResized()) {
		window.setFramebufferResized(false);
		swapchain.recreateSwapChain(renderpass.getRenderPass(), depth.getDepthImageView());
		color.createColorResources();
		depth.createDepthResources();
		camera.setAspect(swapchain.getExtent().width / (f32) swapchain.getExtent().height);
		swapchain.createFramebuffers(renderpass.getRenderPass(), depth.getDepthImageView(), color.getColorImageView());
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to present swap chain image");
	}

	// Advance to the next frame every cycle
	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

// Wrapper to wait for the device's GPU to be free
void Rath::Renderer::wait() {
	vkDeviceWaitIdle(device.getDevice());
}

// Allocates the command buffer with the command pool
// Doesn't need to be deleted since it is freed when the command pool
// is deleted
void Rath::Renderer::createCommandBuffers() {
	commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<u32>(commandBuffers.size());

	if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate command buffers");
	}

	computeCommandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

	allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = device.getCommandPool();
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<u32>(computeCommandBuffers.size());

	if (vkAllocateCommandBuffers(device.getDevice(), &allocInfo, computeCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate compute command buffers");
	}
}

// Creates the semaphores and fences needed for rendering
// TODO: function to resize renderFinishedSemaphores after swapchain recreation
void Rath::Renderer::createSyncObjects() {
	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	// TODO: function to resize renderFinishedSemaphores after swapchain recreation
	renderFinishedSemaphores.resize(swapchain.getImageCount());
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	computeFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	computeInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &computeFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device.getDevice(), &fenceInfo, nullptr, &computeInFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create sync objects");
		}
	}
	for (size i = 0; i < swapchain.getImageCount(); i++) {
		if (vkCreateSemaphore(device.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create renderFinishedSemaphore");
			}
	}
}

// The recording of the command buffer
// Begins command buffer recording -> starts renderpass -> bind the pipeline
// sets the viewport and scissor (since they are dynamic states)
// then binds the vertex, descriptorSets, index to the command buffer before drawing
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

	// Same to the order of my renderpass attachments
	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<u32>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	//vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.height = static_cast<f32>(swapchain.getExtent().height);
	viewport.width = static_cast<f32>(swapchain.getExtent().width);
	viewport.maxDepth = 1.0f;
	viewport.minDepth = 0.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0,0 };
	scissor.extent = swapchain.getExtent();
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	// per frame bind (set 0)
	VkDescriptorSet set = descriptor.getUBOSet(currentFrame);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipelineLayout(),
							0, 1, &set, 0, nullptr);

	rModel.bind(commandBuffer);
	rModel.bindPipeline(commandBuffer);

	// per material bind
	rModel.bindDescriptors(commandBuffer);

	for (size i = 0; i < NUMBER_OF_ROOMS; i++) {
		// A * B * v
		// this applies B first so you rotate THEN translate
		MeshPushConstant constants{ enginemath::Mat4::translationM(enginemath::Vec3(2.0f * i, 0.0f, 1.0f)) *
			enginemath::Mat4::rotateY(camera.getElapsedTime() * enginemath::toRad(90.0f)) *
			enginemath::Mat4::rotateX(enginemath::toRad(-90.0f)),
									enginemath::Vec3(std::sin(i * enginemath::toRad(camera.getElapsedTime())), 1.0f, 1.0f)};

		vkCmdPushConstants(commandBuffer, pipeline.getPipelineLayout(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(MeshPushConstant), &constants);
		rModel.draw(commandBuffer);
	}

	cupModel.bind(commandBuffer);
	cupModel.bindPipeline(commandBuffer);
	cupModel.bindDescriptors(commandBuffer);
	cupModel.draw(commandBuffer);
	
	// DRAW THE PARTICLES VIA STORAGE BUFFER AND PARTICLE PIPELINE
	VkDescriptorSet particleSet = descriptor.getUBOSet(currentFrame);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getParticlePipelineLayout(),
		0, 1, &particleSet, 0, nullptr);
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getParticlePipeline());

	VkBuffer particleBuffers[] = { storage.getStorageBuffer(currentFrame) };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, particleBuffers, offsets);
	vkCmdDraw(commandBuffer, PARTICLE_COUNT, 1, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to record command buffer");
	}
}

// Begins a command buffer (compute), binds compute descriptor set, then dispatches
void Rath::Renderer::recordComputeCommandBuffer(VkCommandBuffer commandBuffer) {
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("Failed to begin command buffer");
	}

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.getComputePipeline());

	VkDescriptorSet set = descriptor.getComputeSet(currentFrame);

	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.getComputePipelineLayout(),
							0, 1, &set, 0, nullptr);

	vkCmdDispatch(commandBuffer, PARTICLE_COUNT / 256, 1, 1);

	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to end command buffer");
	}
}

void Rath::Renderer::setUpModel(const std::string modelPath, const std::string texturePath, R_Material* material, Model* model) {
	R_ModelMaterialCreateInfo rMaterialCreateInfo{};
	rMaterialCreateInfo.texturePath = texturePath;

	if (!R_Material::rCreateMaterial(device, buffer, descriptor, image, rMaterialCreateInfo, material)) {
		throw std::runtime_error("Failed to create material for model");
	}

	R_ModelCreateInfo rModelInfo{};
	rModelInfo.pipeline = pipeline.getGraphicsPipeline();
	rModelInfo.pipelineLayout = pipeline.getPipelineLayout();
	rModelInfo.material = material;
	rModelInfo.modelPath = modelPath;

	if (!Model::rCreateModel(device, buffer, rModelInfo, model)) {
		throw std::runtime_error("Failed to create model");
	}
}