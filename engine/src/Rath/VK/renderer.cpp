#include "renderer.hpp"

RATH::Renderer::Renderer(Window& window) : 
	device(window), ctx(buildCtx()) {

}

RATH::Renderer::~Renderer() {

}

RATH::vkCtx RATH::Renderer::buildCtx() {
	vkCtx ctx{
		.instance = context.getInstance(),
	};

	return ctx;
}