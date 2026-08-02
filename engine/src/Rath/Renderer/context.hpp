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

			// Returns instance
			VkInstance getInstance() const { return instance; };

			// Returns surface
			VkSurfaceKHR getSurface() const { return surface; };

		private:
			Window& window;
			VkInstance instance = VK_NULL_HANDLE;
			VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
			VkSurfaceKHR surface = VK_NULL_HANDLE;

			// Creates the instance connecting the application to Vulkan,
			// specifies the RATH version and obtains the extensions
			void createInstance();

			// Checks if the instance supports the layers in validationLayers
			bool checkValidationLayerSupport();

			// Returns the extensions glfw requires, plus the debug utils
			// extension when RATH_DEBUG is enabled
			std::vector<const char*> getRequiredExtensions();

			// Fills out the debug messenger struct, specifies which message
			// types and severities are shown
			void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

			// Sets up the debug messenger only if RATH_DEBUG is enabled
			void setupDebugMessenger();

			// Creates the interface connection between the instance and the glfw window
			void createSurface();
	};
} // namespace Rath