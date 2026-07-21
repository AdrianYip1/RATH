#include "context.hpp"

// Context Constructor
Rath::Context::Context() {
	createInstance();
}

// Context Destructor
Rath::Context::~Context() {
	vkDestroyInstance(instance, nullptr);
	std::cout << "Deleted Vulkan instance" << std::endl;
}

// Create the instance to connect application to Vulkan
// Specify RATH version here + obtain extensions
void Rath::Context::createInstance() {
	// Version of RATH: change as needed
	constexpr u32 RATH_VERSION = VK_MAKE_VERSION(0, 0, 1);

	u32 glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "RATH Engine";
	appInfo.applicationVersion = RATH_VERSION;
	appInfo.pEngineName = "RATH Engine";
	appInfo.engineVersion = RATH_VERSION;
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance");
	}

	std::cout << "Created Vulkan instance" << std::endl;
}