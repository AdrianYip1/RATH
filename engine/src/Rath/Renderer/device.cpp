#include "device.hpp"

Rath::Device::Device(Context& _context) : context(_context) {
	std::cout << "Created device" << std::endl;
}

Rath::Device::~Device() {
	std::cout << "Deleted device" << std::endl;
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

bool Rath::Device::isDeviceSuitable(VkPhysicalDevice device) {
	QueueFamilyIndices indices = findQueueFamilies(device);

	return indices.isComplete();
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
			indices.graphicsFamily = i++;
		}
		// Early Exit
		if (indices.isComplete()) {
			break;
		}
	}

	return indices;
}
