// Contains physical/logical devices, queues and their indices
#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "context.hpp"

// std
#include <map>
#include <set>
#include <optional>
#include <string>


namespace Rath {

	const std::vector<const char*> deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	struct QueueFamilyIndices {
		std::optional<u32> graphicsFamily;
		std::optional<u32> presentFamily;

		bool isComplete() {
			return graphicsFamily.has_value() && 
				   presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class Device {
		public:
			Device(Context& context);
			~Device();
			Device(const Device& other) = delete;
			Device& operator=(const Device& other) = delete;

			VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; };
			VkDevice getDevice() const { return device; };
			VkQueue getGraphicsQueue() const { return graphicsQueue; };
			VkQueue getPresentQueue() const { return presentQueue; };
			VkCommandPool getCommandPool() const { return commandPool; };

			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
			VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates,
			VkImageTiling tiling, VkFormatFeatureFlags features);

			bool hasStencilComponent(VkFormat format);

		private:
			const Context& context;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkDevice device = VK_NULL_HANDLE;
			// Retrieve queue handles using vkGetDeviceQueue
			VkQueue graphicsQueue = VK_NULL_HANDLE;
			VkQueue presentQueue = VK_NULL_HANDLE;
			VkCommandPool commandPool;

			void pickPhysicalDevice();
			void createLogicalDevice();
			bool isDeviceSuitable(VkPhysicalDevice device);
			bool checkDeviceExtensionSupport(VkPhysicalDevice device);
			void createCommandPool();
		
	};
} // namespace Rath