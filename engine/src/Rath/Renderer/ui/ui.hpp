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

// std
#include <map>
#include <set>
#include <optional>
#include <string>

namespace Rath {
	class UI {
		public:
			UI();
			~UI();
			UI(const UI& other) = delete;
			UI& operator=(const UI& other);

		private:

	};
} // namespace Rath