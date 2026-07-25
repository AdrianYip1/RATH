#include "renderer.hpp"

Rath::Renderer::Renderer(Window& window) : 
	context(window), 
	device(context), 
	swapchain(window, context, device),
	renderpass(device, swapchain),
	pipeline(device, swapchain, renderpass) {

}

Rath::Renderer::~Renderer() {

}