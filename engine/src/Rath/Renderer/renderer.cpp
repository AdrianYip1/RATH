#include "renderer.hpp"

Rath::Renderer::Renderer(Window& window) : 
	context(window), 
	device(context), 
	swapchain(window, context, device),
	pipeline() {

}

Rath::Renderer::~Renderer() {

}