#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <Rath/Core/defines.hpp>

namespace RATH {

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	class Context {
	public:
		Context();
		~Context();
		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

		VkInstance getInstance() { return instance; };

	private:
		void createInstance();
		bool checkValidationLayerSupport();
		std::vector<const char*> getRequiredExtensions();

		void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugCreateInfo);
		void setupDebug();

		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

	};
} // namespace RATH