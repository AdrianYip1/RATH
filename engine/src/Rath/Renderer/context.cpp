#include "context.hpp"

// First 3 functions are proxy functions for the debug messenger:

// Debug callback
static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) {

	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

	return VK_FALSE;
}

// Debug messenger
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

// Debug destroy
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, debugMessenger, pAllocator);
	}
}

// Context Constructor
Rath::Context::Context(Window& _window) : window(_window) {
	createInstance();
	setupDebugMessenger();
	createSurface();
}

// Context Destructor
Rath::Context::~Context() {
	if (RATH_DEBUG) {
		DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		std::cout << "Deleted debug messenger" << std::endl;
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	std::cout << "Deleted surface" << std::endl;
	vkDestroyInstance(instance, nullptr);
	std::cout << "Deleted Vulkan instance" << std::endl;
}

// Returns the VkInstance instance object
VkInstance Rath::Context::getInstance() const {
	return instance;
}

// Returns the vulkan surface
VkSurfaceKHR Rath::Context::getSurface() const {
	return surface;
}

// Create the instance to connect application to Vulkan
// Specify RATH version here + obtain extensions
void Rath::Context::createInstance() {
	if (RATH_DEBUG && !checkValidationLayerSupport()) {
		throw std::runtime_error("Validation layers not available but requested");
	}

	// Version of RATH: change as needed
	constexpr u32 RATH_VERSION = VK_MAKE_VERSION(0, 0, 1);

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

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<u32>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();;

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (RATH_DEBUG) {
		createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		populateDebugMessengerCreateInfo(debugCreateInfo);
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance");
	}

	std::cout << "Created Vulkan instance" << std::endl;
}

//
bool Rath::Context::checkValidationLayerSupport() {
	u32 layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperty : availableLayers) {
			if (strcmp(layerName, layerProperty.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) return false;
	}

	return true;
}

//
std::vector<const char*> Rath::Context::getRequiredExtensions() {
	u32 glfwExtensionCount = 0;
	const char** glfwExtensions;

	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

	if (RATH_DEBUG) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

//
void Rath::Context::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

	// VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT not used 
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
}

//
void Rath::Context::setupDebugMessenger() {
	if (!RATH_DEBUG) return;

	VkDebugUtilsMessengerCreateInfoEXT createInfo;
	populateDebugMessengerCreateInfo(createInfo);

	if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
		throw std::runtime_error("Failed to setup debug messenger");
	}
}

void Rath::Context::createSurface() {
	if (glfwCreateWindowSurface(instance, window.getWindow(), nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create window surface");
	}
}