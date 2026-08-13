#pragma once
#include <vulkan/vulkan.h>
// core api
#include <imgui.h> 
// input (mouse pos, clicks, keys, windowsize from glfw into imgui)
#include <imgui_impl_glfw.h> 
// output (imgui's list of shapes into vulkan calls
#include <imgui_impl_vulkan.h>
#include <GLFW/glfw3.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "Rath/Platform/window.hpp"
#include "../context.hpp"
#include "../device.hpp"
#include "../pipeline.hpp"
#include "../swapchain.hpp"
#include "../renderpass.hpp"

// std
#include <map>
#include <set>
#include <optional>
#include <string>

namespace Rath {
	// Placeholder for push constants -> drawing multiple objects 
	inline i32 NUMBER_OF_ROOMS = 4;
	class UI {
		public:
			UI(Window& _window, Context& _context, Device& _device, 
				Renderpass& _renderpass, Swapchain& _swapchain, Pipeline& _pipeline);
			~UI();
			UI(const UI& other) = delete;
			UI& operator=(const UI& other) = delete;

			void startFrame();
			void drawUI();
			void draw(VkCommandBuffer commandBuffer);

		private:
			Window& window;
			Context& context;
			Device& device;
			Renderpass& renderpass;
			Swapchain& swapchain;
			Pipeline& pipeline;

	};
} // namespace Rath