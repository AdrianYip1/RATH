#include "device.hpp"

RATH::Device::Device(Context& _context) : 
	context(_context) {
	pickPhysicalDevice();
	createLogicalDevice();
}

RATH::Device::~Device() {
	vkDestroyDevice(device, nullptr);
}

void RATH::Device::pickPhysicalDevice() {
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(context.getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) throw std::runtime_error("ERROR: device.cpp -> pickPhysicalDevice() -> deviceCount == 0");

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(context.getInstance(), &deviceCount, devices.data());

	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}
	if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("ERROR: device.cpp -> pickPhysicalDevice() -> failed to find physical device");

	queueFamilyIndices = findQueueFamilies(physicalDevice);
}

bool RATH::Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapchainSupportDetails swapChainSupport = querySwapchainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate &&
		supportedFeatures.samplerAnisotropy &&
		supportedFeatures.sampleRateShading;
}

RATH::QueueFamilyIndices RATH::Device::findQueueFamilies(VkPhysicalDevice device) {
	QueueFamilyIndices indices;

	u32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT &&
			queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
			indices.graphicsAndComputeFamily = i;
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

void RATH::Device::createLogicalDevice() {
	QueueFamilyIndices indices = queueFamilyIndices;

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<u32> uniqueQueueFamilies = { indices.graphicsAndComputeFamily.value(),
										  indices.presentFamily.value() };
	f32 queuePriority = 1.0f;

	for (u32 queueFamily : uniqueQueueFamilies) {

		VkDeviceQueueCreateInfo queueCreateInfo{
			.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			.queueFamilyIndex = queueFamily,
			.queueCount = 1,
			.pQueuePriorities = &queuePriority,
		};

		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures {
		.sampleRateShading = VK_TRUE,
		.samplerAnisotropy = VK_TRUE,
	};

	VkDeviceCreateInfo deviceCreateInfo {
		.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
		.queueCreateInfoCount = static_cast<u32>(queueCreateInfos.size()),
		.pQueueCreateInfos = queueCreateInfos.data(),
		.enabledExtensionCount = static_cast<u32>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data(),
		.pEnabledFeatures = &deviceFeatures,
	};

	if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: device.cpp -> createLogicalDevice() -> vkCreateDevice()");
	}

	vkGetDeviceQueue(device, indices.graphicsAndComputeFamily.value(), 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

bool RATH::Device::checkDeviceExtensionSupport(VkPhysicalDevice device) {
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

RATH::SwapchainSupportDetails RATH::Device::querySwapchainSupport(VkPhysicalDevice device) {
	SwapchainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, context.getSurface(), &details.capabilities);

	u32 formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, context.getSurface(), &formatCount, nullptr);
	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, context.getSurface(), &formatCount, details.formats.data());
	}

	u32 presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, context.getSurface(), &presentModeCount, nullptr);
	if (presentModeCount != 0) {
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, context.getSurface(), &presentModeCount, details.presentModes.data());
	}

	return details;
}
