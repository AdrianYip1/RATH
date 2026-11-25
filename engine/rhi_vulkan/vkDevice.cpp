#include "vkDevice.hpp"

#ifdef RATH_DEBUG
static constexpr bool enableValidation = true;
#else
static constexpr bool enableValidation = false;
#endif


#include <GLFW/glfw3.h>

// Extensions for the physical device
static const std::vector<const char*> requiredDeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo() {
	return {
		.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
		.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
		.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
		.pfnUserCallback = [](VkDebugUtilsMessageSeverityFlagBitsEXT,
			VkDebugUtilsMessageTypeFlagsEXT,
			const VkDebugUtilsMessengerCallbackDataEXT* data,
			void*) -> VkBool32 {
			std::cerr << "[vulkan] " << data->pMessage << '\n';
			return VK_FALSE;
		},
	};
}

RATH::Device::Device(GLFWwindow* window) :
	instance(createInstance()),
	debugMessenger(createDebugMessenger(instance)),
	surface(createSurface(instance, window)),
	physicalDevice(selectPhysicalDevice(instance, surface)),
	queueInfos(findQueueFamilies(physicalDevice, surface).value()),
	logicalDevice(selectLogicalDevice(physicalDevice, queueInfos))
{
	vkGetDeviceQueue(logicalDevice, queueInfos.graphicsQueueIdx.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(logicalDevice, queueInfos.presentQueueIdx.value(), 0, &presentQueue);
	vkGetDeviceQueue(logicalDevice, queueInfos.computeQueueIdx.value(), 0, &computeQueue);

	transientCommandPool = createTransientCommandPool(logicalDevice, queueInfos.graphicsQueueIdx.value());
}

RATH::Device::~Device() {
	vkDestroyCommandPool(logicalDevice, transientCommandPool, nullptr);
	vkDestroyDevice(logicalDevice, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	if (debugMessenger) {
		auto destroyMessenger = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
			vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
		destroyMessenger(instance, debugMessenger, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
}

VkInstance RATH::Device::createInstance() {
	VkInstance instance = VK_NULL_HANDLE;

	u32 glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	if (!glfwExtensions) throw std::runtime_error("ERROR: vkDevice.cpp -> createInstance()");
	
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	std::vector<const char*> layers;

	if (enableValidation) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		layers.push_back("VK_LAYER_KHRONOS_validation");

		u32 availableCount = 0;
		vkEnumerateInstanceLayerProperties(&availableCount, nullptr);
		std::vector<VkLayerProperties> available(availableCount);
		vkEnumerateInstanceLayerProperties(&availableCount, available.data());

		for (const char* layer : layers) {
			bool found = std::any_of(available.begin(), available.end(),
				[&](const VkLayerProperties& p) { return std::strcmp(p.layerName, layer) == 0; });
			if (!found) {
				throw std::runtime_error(std::string("ERROR: vkDevice.cpp -> createInstance()") + layer);
			}
		}
	}

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = debugMessengerInfo();

	VkApplicationInfo appInfo = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Photo Visualizer Software",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "Vulkan Custom Engine",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_3
	};
	VkInstanceCreateInfo instanceCreateInfo{
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pNext = enableValidation ? &debugCreateInfo : nullptr,
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<u32>(layers.size()),
		.ppEnabledLayerNames = layers.data(),
		.enabledExtensionCount = static_cast<u32>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
	};

	if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkDevice.cpp -> createInstance()");
	}

	return instance;
}

VkDebugUtilsMessengerEXT RATH::Device::createDebugMessenger(VkInstance instance) {
	if (!enableValidation) return VK_NULL_HANDLE;
	

	auto createMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
		vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));

	VkDebugUtilsMessengerCreateInfoEXT createInfo = debugMessengerInfo();
	VkDebugUtilsMessengerEXT messenger = VK_NULL_HANDLE;
	if (!createMessenger || createMessenger(instance, &createInfo, nullptr, &messenger) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkDevice.cpp -> createDebugMessenger()");
	}
	return messenger;
}

VkSurfaceKHR RATH::Device::createSurface(VkInstance instance, GLFWwindow* window) {
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	
	if (glfwCreateWindowSurface(instance, window , nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkDevice.cpp -> createSurface()");
	}

	return surface;
}

VkPhysicalDevice RATH::Device::selectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (deviceCount == 0) throw std::runtime_error("ERROR: vkDevice.cpp -> selectPhysicalDevice(): no Vulkan GPU found");
	
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

	VkPhysicalDevice chosenDevice = VK_NULL_HANDLE;
	for (auto device : devices) {
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(device, &properties);
		if (properties.apiVersion < VK_API_VERSION_1_3) continue;

		// Check for Vulkan Features
		VkPhysicalDeviceVulkan13Features features13 = { 
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES 
		};
		VkPhysicalDeviceFeatures2 features2 = { 
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, 
			.pNext = &features13
		};
		vkGetPhysicalDeviceFeatures2(device, &features2);
		if (!features13.dynamicRendering || !features13.synchronization2) continue;

		// Check for queues
		if (!findQueueFamilies(device, surface).has_value()) continue;

		// Check for extensions
		if (!extensionsSuitable(device)) continue;

		// Swapchain adequacy
		SwapchainSupportDetails swapchainSupport = querySwapchainSupport(device, surface);
		if (swapchainSupport.formats.empty() || swapchainSupport.presentModes.empty()) continue;

		chosenDevice = device;
		// Use discrete if found right way
		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) break;
	} 

	if (chosenDevice == VK_NULL_HANDLE) throw std::runtime_error("ERROR: vkDevice.cpp -> selectPhysicalDevice()");
	return chosenDevice;
}

std::optional<RATH::QueueInfos> RATH::Device::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {
	u32 familyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
	std::vector<VkQueueFamilyProperties> properties(familyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, properties.data());

	std::optional<u32> graphics, present, compute;
	for (u32 i = 0; i < familyCount; i++) {
		if (!graphics && properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) graphics = i;
		if (!compute && properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) compute = i;

		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
		if (!present && presentSupport) present = i;
	}

	if (!graphics || !compute || !present) return std::nullopt;
	return QueueInfos{
		.graphicsQueueIdx = graphics,
		.presentQueueIdx = present,
		.computeQueueIdx = compute
	};
}

bool RATH::Device::extensionsSuitable(VkPhysicalDevice device) {
	u32 deviceExtensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, nullptr);

	std::vector<VkExtensionProperties> extensions(deviceExtensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &deviceExtensionCount, extensions.data());
	
	return std::all_of(requiredDeviceExtensions.begin(), requiredDeviceExtensions.end(),
		[&](const char* required) {
			return std::any_of(extensions.begin(), extensions.end(),
				[&](const VkExtensionProperties& e) {
					return std::strcmp(e.extensionName, required) == 0;
				});
		});
}

VkDevice RATH::Device::selectLogicalDevice(VkPhysicalDevice physicalDevice, const QueueInfos& queueInfo) {
	VkDevice logicalDevice = VK_NULL_HANDLE;
	f32 priority = 1.0f;

	std::set<u32> uniqueFamilies = {
		queueInfo.graphicsQueueIdx.value(),
		queueInfo.presentQueueIdx.value(),
		queueInfo.computeQueueIdx.value()
	};

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	for (auto family : uniqueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = family,
			.queueCount = 1,
			.pQueuePriorities = &priority
		};
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceVulkan13Features features13 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
		.synchronization2 = VK_TRUE,
		.dynamicRendering = VK_TRUE
	};

	VkPhysicalDeviceFeatures2 features2 = {
		.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
		.pNext = &features13
	};

	VkDeviceCreateInfo createInfo = {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.pNext = &features2,
		.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.enabledExtensionCount = static_cast<u32>(requiredDeviceExtensions.size()),
		.ppEnabledExtensionNames = requiredDeviceExtensions.data(),
		.pEnabledFeatures = nullptr

	};

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkDevice.cpp -> selectLogicalDevice()");
	}

	return logicalDevice;
}

RATH::SwapchainSupportDetails RATH::Device::querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface) {
	SwapchainSupportDetails swapchainSupport;

	u32 formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
	if (formatCount > 0) {
		swapchainSupport.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapchainSupport.formats.data());
	} 

	u32 presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
	if (presentModeCount > 0) {
		swapchainSupport.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, swapchainSupport.presentModes.data());
	}

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapchainSupport.capabilities);
	return swapchainSupport;
}

VkCommandPool RATH::Device::createTransientCommandPool(VkDevice logicalDevice, u32 queueFamilyIdx) {
	VkCommandPoolCreateInfo poolCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		// Hint to the driver that command buffers from this pool are short lived
		.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
		.queueFamilyIndex = queueFamilyIdx
	};

	VkCommandPool commandPool = VK_NULL_HANDLE;

	if (vkCreateCommandPool(logicalDevice, &poolCreateInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkDevice.cpp -> createTransientCommandPool()");
	}

	return commandPool;
}

VkCommandBuffer RATH::Device::beginSingleTimeCommands() {
	VkCommandBufferAllocateInfo allocInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = transientCommandPool,
		.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
		.commandBufferCount = 1
	};

	VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
	if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkDevice.cpp -> beginSingleTimeCommands() allocate");
	}

	VkCommandBufferBeginInfo beginInfo = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};

	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
		vkFreeCommandBuffers(logicalDevice, transientCommandPool, 1, &commandBuffer);
		throw std::runtime_error("ERROR: vkDevice.cpp -> beginSingleTimeCommands() begin");
	}

	return commandBuffer;
}

void RATH::Device::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
		vkFreeCommandBuffers(logicalDevice, transientCommandPool, 1, &commandBuffer);
		throw std::runtime_error("ERROR: vkDevice.cpp -> endSingleTimeCommands() end");
	}

	VkSubmitInfo submitInfo = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &commandBuffer
	};

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		vkFreeCommandBuffers(logicalDevice, transientCommandPool, 1, &commandBuffer);
		throw std::runtime_error("ERROR: vkDevice.cpp -> endSingleTimeCommands() submit");
	}

	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(logicalDevice, transientCommandPool, 1, &commandBuffer);
}
