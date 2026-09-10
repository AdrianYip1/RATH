#pragma once
#include <vulkan/vulkan.h>
#include <Rath/Core/defines.hpp>

#include "../Platform/window.hpp"
#include "context.hpp"

namespace RATH {

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	struct QueueFamilyIndices {
		std::optional<u32> graphicsAndComputeFamily;
		std::optional<u32> presentFamily;

		bool isComplete() {
			return graphicsAndComputeFamily.has_value() &&
				presentFamily.has_value();
		}
	};

	class Device {
	public:
		Device(Context& _context);
		~Device();
		Device(const Device& other) = delete;
		Device& operator=(const Device& other) = delete;

		VkDevice getDevice() { return device; };
		VkPhysicalDevice getPhysicalDevice() { return physicalDevice; };
		QueueFamilyIndices getQueueFamilyIndices() { return queueFamilyIndices; };
		SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);

	private:
		Context& context;

		VkDevice device = VK_NULL_HANDLE;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;

		QueueFamilyIndices queueFamilyIndices;
		
		void pickPhysicalDevice();
		bool isDeviceSuitable(VkPhysicalDevice device);
		RATH::QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
		bool checkDeviceExtensionSupport(VkPhysicalDevice device);
		void createLogicalDevice();
	};
} // namespace RATH