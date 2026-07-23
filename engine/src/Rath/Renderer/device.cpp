#include "device.hpp"

Rath::Device::Device(Context& _context) : context(_context) {
	std::cout << "Created device" << std::endl;
	pickPhysicalDevice();
	createLogicalDevice();
}

Rath::Device::~Device() {
	vkDestroyDevice(device, nullptr);
	std::cout << "Deleted device" << std::endl;
}

VkPhysicalDevice Rath::Device::getPhysicalDevice() {
	return physicalDevice;
}

VkDevice Rath::Device::getDevice() {
	return device;
}

VkQueue Rath::Device::getGraphicsQueue() {
	return graphicsQueue;
}

VkQueue Rath::Device::getPresentQueue() {
	return presentQueue;
}

// TODO: instead of going with the 1st suitable device, give each device a
// score and choose the highest scoring device
void Rath::Device::pickPhysicalDevice() {
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(context.getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(context.getInstance(), &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("Failed to find suitable GPU");
	}
}

void Rath::Device::createLogicalDevice() {
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<u32> uniqueQueueFamilies = { indices.graphicsFamily.value(),
										  indices.presentFamily.value() };
	float queuePriority = 1.0f;

	for (u32 queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures{};

	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size());
	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<u32>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	/* enabledLayerCount and ppEnabledLayerNames are legacy and not used
	if (RATH_DEBUG) {
		createInfo.enabledLayerCount = static_cast<u32>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}
	*/

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create logical device");
	}

	vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

bool Rath::Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

bool Rath::Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
	u32 extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

Rath::QueueFamilyIndices Rath::Device::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		VkBool32 presentSupport = VK_FALSE;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, context.getSurface(), &presentSupport);
		if (presentSupport) {
			indices.presentFamily = i;
		}

		i++;
		// Early Exit
		if (indices.isComplete()) {
			break;
		}
	}

	return indices;
}

Rath::SwapChainSupportDetails Rath::Device::querySwapChainSupport(VkPhysicalDevice device) {
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, context.getSurface(), &details.capabilities);

	u32 formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, context.getSurface(), &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, context.getSurface(), &formatCount, details.formats.data());
	}

	u32 presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, context.getSurface(), &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, context.getSurface(), &presentModeCount, details.presentModes.data());
	}

	return details;
}