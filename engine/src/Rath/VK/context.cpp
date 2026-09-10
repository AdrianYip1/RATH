#include "context.hpp"

RATH::Context::Context() {
	createInstance();
}

RATH::Context::~Context() {
	vkDestroyInstance(instance, nullptr);
}

void RATH::Context::createInstance() {
	u32 glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkApplicationInfo appInfo{
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "RATH",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "RATHENGINE",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_VERSION_1_0,
	};

	VkInstanceCreateInfo instanceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = 0,
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions,
	};

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: context.cpp -> createInstance() -> vkCreateInstance() failed");
	}
}