#include "light.hpp"

Rath::R_Light::R_Light(Device& _device, Swapchain& _swapchain, Buffer& _buffer) :
	device(_device), swapchain(_swapchain), buffer(_buffer) {

	createLightBuffers();
}

Rath::R_Light::~R_Light() {
	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(device.getDevice(), lightBuffers[i], nullptr);
		vkFreeMemory(device.getDevice(), lightBuffersMemory[i], nullptr);
	}
}

void Rath::R_Light::updateLights(u32 currentImage, const std::unordered_map<u32, R_SceneLight>& lights) {

	size lightCount = std::min(lights.size(), (size)MAX_LIGHTS);

	R_LightUbo lightUbo{};
	size i = 0;
	for (const auto& [id, light] : lights) {
		// since the ids may not be in order, check if the number of updated
		// lights exceeds MAX_LIGHTS
		if (i >= MAX_LIGHTS) break;
		lightUbo.lights[i].position = light.position;
		lightUbo.lights[i].color = light.color;
		i++;
	}

	lightUbo.lightCount = i;

	memcpy(lightBuffersMapped[currentImage], &lightUbo, sizeof(lightUbo));
	
}

void Rath::R_Light::createLightBuffers() {
	VkDeviceSize bufferSize = sizeof(R_LightUbo);

	lightBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	lightBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	lightBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		buffer.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
							VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							lightBuffers[i], lightBuffersMemory[i]);

		vkMapMemory(device.getDevice(), lightBuffersMemory[i], 0, bufferSize, 0, &lightBuffersMapped[i]);
	}
}