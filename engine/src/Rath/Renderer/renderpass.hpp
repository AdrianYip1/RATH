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

		VkRenderPass getRenderPass();
	private:
		Device& device;
		Swapchain& swapchain;
		Depth& depth;

		VkRenderPass renderPass;

		void createRenderPass();
		
	};
} // namespace Rath