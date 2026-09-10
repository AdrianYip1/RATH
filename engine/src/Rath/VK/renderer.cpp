#include "renderer.hpp"

RATH::Renderer::Renderer(Window& window) : 
	context(window), device(context), ctx(buildCtx()) {

}

RATH::Renderer::~Renderer() {

}

RATH::vkCtx RATH::Renderer::buildCtx() {
	vkCtx ctx{
		.instance = context.getInstance(),
		.physical = device.getPhysicalDevice(),
		.device = device.getDevice(),
		.surface = context.getSurface()
	};

	return ctx;
}