#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "device.hpp"
#include "swapchain.hpp"
#include "images/depth.hpp"
#include "Rath/Core/defines.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <map>
#include <set>
#include <optional>
#include <string>
#include <fstream>
#include <vector>

namespace Rath {
	class Renderpass {
	public:
		Renderpass(Device& _device, Swapchain& _swapchain, Depth& _depth);
		~Renderpass();
		Renderpass(const Renderpass& other) = delete;
		Renderpass& operator=(const Renderpass& other) = delete;

		// Returns renderPass
		VkRenderPass getRenderPass() const { return renderPass; };

	private:
		Device& device;
		Swapchain& swapchain;
		Depth& depth;

		VkRenderPass renderPass;

		// Creates the render pass from the colour and depth attachment descriptions,
		// their references, the single subpass, and the subpass dependency that
		// orders this pass after everything before it
		void createRenderPass();
		
	};
} // namespace Rath