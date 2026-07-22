// Contains instance, debug/validation, and surfaces
#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>

#include "Rath/Core/defines.hpp"

namespace Rath {
	// Standard validation
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	class Context {
		public:
			Context();
			~Context();
			Context(const Context& other) = delete;
			Context& operator=(const Context& other) = delete;

			VkInstance getInstance() const;
		private:
			VkInstance instance;
			VkDebugUtilsMessengerEXT debugMessenger;

			void createInstance();
			bool checkValidationLayerSupport();
			std::vector<const char*> getRequiredExtensions();
			void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
			void setupDebugMessenger();
	};
} // namespace Rath