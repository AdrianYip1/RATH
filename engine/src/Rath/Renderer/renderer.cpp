#include "renderer.hpp"

Rath::Renderer::Renderer(Window& window) : 
	context(window), device(context), swapchain(window, context, device) {

}

Rath::Renderer::~Renderer() {

}