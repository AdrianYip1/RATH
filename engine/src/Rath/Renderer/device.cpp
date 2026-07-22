#include "device.hpp"

Rath::Device::Device(Context& _context) : context(_context) {
	std::cout << "Created device" << std::endl;
}

Rath::Device::~Device() {
	std::cout << "Deleted device" << std::endl;
}

void Rath::Device::pickPhysicalDevice() {
	u32 deviceCount = 0;
	vkEnumeratePhysicalDevices(context.getInstance(), &deviceCount, nullptr);

	if (deviceCount == 0) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support");
	}
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(context.getInstance(), &deviceCount, devices.data());


}

bool Rath::Device::isDeviceSuitable() {
	return true;
}

Rath::u32 Rath::Device::findQueueFamilies(VkPhysicalDevice device) {
	return 0;
}
