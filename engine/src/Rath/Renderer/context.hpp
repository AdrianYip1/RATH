// Contains instance, debug/validation, and surfaces
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "Rath/Platform/window.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>


namespace Rath {
	// Standard validation
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	class Context {
		public:
			Context(Window& _window);
			~Context();
			Context(const Context& other) = delete;
			Context& operator=(const Context& other) = delete;

			VkInstance getInstance() const;
			VkSurfaceKHR getSurface() const;
		private:
			Window& window;
			VkInstance instance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
			VkSurfaceKHR surface = VK_NULL_HANDLE;

			void createInstance();
			bool checkValidationLayerSupport();
			std::vector<const char*> getRequiredExtensions();
			void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
			void setupDebugMessenger();
			void createSurface();
	};
} // namespace Rath