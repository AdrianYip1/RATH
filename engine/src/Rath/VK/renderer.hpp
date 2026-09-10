#pragma once
#include <vulkan/vulkan.h>

#include "Rath/Platform/window.hpp"
#include "Rath/VK/context.hpp"
#include "Rath/VK/device.hpp"
#include "Rath/VK/vkCtx.hpp"

namespace RATH {
	class Renderer {
	public:
		Renderer(Window& window);
		~Renderer();
		Renderer(const Renderer& other) = delete;
		Renderer& operator=(const Renderer& other) = delete;

	private:
		Context context;   
		Device device;
		vkCtx ctx;  

		vkCtx buildCtx();
	};
} // namespace RATH
