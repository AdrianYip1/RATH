#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <Rath/Core/defines.hpp>

#include "../Platform/window.hpp"

namespace RATH {

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	class Context {
	public:
		Context(Window& _window);
		~Context();
		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		VkInstance getInstance() { return instance; };
		VkSurfaceKHR getSurface() { return surface; };

	private:
		void createInstance();
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);
		void setupDebug();
		void createSurface();

		Window& window;
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
	};
} // namespace RATH
