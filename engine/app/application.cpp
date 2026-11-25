#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "application.hpp"
#include "../platform/window.hpp"
#include "../rhi_vulkan/vkDevice.hpp"
#include "../rhi_vulkan/vkSwapchain.hpp"
#include "../rhi_vulkan/vkPipeline.hpp"
#include "../rhi_vulkan/vkRender.hpp"
#include "../rhi_vulkan/vkBuffer.hpp"
#include "../rhi_vulkan/vkVertex.hpp"

static const std::vector<RATH::Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

static const std::vector<RATH::u16> indices = {
	0, 1, 2, 2, 3, 0
};

RATH::Application::Application(const ApplicationConfig& config) :
	window(std::make_unique<Window>(config.width, config.height, config.title)),
	device(std::make_unique<Device>(window->getWindow())),
	swapchain(std::make_unique<Swapchain>(window->getWindow(), *device)),
	pipeline(std::make_unique<Pipeline>(*device, GraphicsPipelineConfig{
				.vertexShaderFilePath = "shaders/basic.vert.spv",
				.fragmentShaderFilePath = "shaders/basic.frag.spv",
				.colorFormat = swapchain->getFormat(),
			})),
	render(std::make_unique<Render>(*device, *swapchain, *pipeline))
{
	// handle vertex buffers
	VkDeviceSize bytes = sizeof(Vertex) * vertices.size();
	auto vertexBuffer = std::make_unique<Buffer>(*device, bytes, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	vertexBuffer->upload(vertices.data(), bytes);
	vertexBuffers.push_back(std::move(vertexBuffer));

	// handle index buffer
	VkDeviceSize indexBytes = sizeof(u16) * indices.size();
	auto indexBuffer = std::make_unique<Buffer>(*device, indexBytes, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	indexBuffer->upload(indices.data(), indexBytes);
	indexBuffers.push_back(std::move(indexBuffer));
}

RATH::Application::~Application() {

}

void RATH::Application::run() {
	onInit();
	while (!window->shouldClose()) {
		window->pollEvents();
		onUpdate(0.0f);

		bool outOfDate = render->drawFrame(*device, *swapchain, *pipeline, vertexBuffers, indexBuffers);
		if (outOfDate || window->getFramebufferResized()) {
			window->setFramebufferResized(false);
			swapchain->recreate();
			render->onSwapchainRecreated(*swapchain);
		}
	}
	vkDeviceWaitIdle(device->getDevice());
	onShutdown();
}
