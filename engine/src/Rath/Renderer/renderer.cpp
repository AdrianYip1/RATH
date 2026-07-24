#include "renderer.hpp"

Rath::Renderer::Renderer(Window& window) : 
	context(window), 
	device(context), 
	swapchain(window, context, device),
	pipeline(device, swapchain) {

}

Rath::Renderer::~Renderer() {

}