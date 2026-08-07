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
		//std::optional<u32> graphicsFamily;
		// Checking for a queue family that supports both 
		// graphics and compute operations
		// TODO: its also possible to have a dedicated compute queue
		// research asynchronous compute queue
		std::optional<u32> graphicsAndComputeFamily;
		std::optional<u32> presentFamily;

		bool isComplete() {
			return graphicsAndComputeFamily.has_value() &&
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

			// Returns physicalDevice
			VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; };

			// Returns device
			VkDevice getDevice() const { return device; };

			// Returns graphicsQueue
			VkQueue getGraphicsQueue() const { return graphicsQueue; };

			// Returns presentQueue
			VkQueue getPresentQueue() const { return presentQueue; };

			// Returns computeQueue
			VkQueue getComputeQueue() const { return computeQueue; };

			// Returns commandPool
			VkCommandPool getCommandPool() const { return commandPool; };

			// Returns msaaSamples
			VkSampleCountFlagBits getMSAASampleCount() { return msaaSamples; };

			// Queries the surface's capabilities, formats, and present modes
			SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

			// Fills out the QueueFamilyIndices struct with the corresponding
			// index values of the queueFamilies
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);

			// Returns the first candidate format that supports the wanted tiling and features
			VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

			// Checks if the format carries a stencil component
			bool hasStencilComponent(VkFormat format);

		private:
			const Context& context;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkDevice device = VK_NULL_HANDLE;
			// Retrieve queue handles using vkGetDeviceQueue
			VkQueue graphicsQueue = VK_NULL_HANDLE;
			VkQueue presentQueue = VK_NULL_HANDLE;
			VkQueue computeQueue = VK_NULL_HANDLE;
			VkCommandPool commandPool;
			VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT;

			// Picks the first suitable physical device
			void pickPhysicalDevice();

			// Creates the logical device from the queue families, device features and
			// device extensions, then retrieves the graphics and present queue handles
			void createLogicalDevice();

			// Determines if a physical device is suitable based on extension support,
			// swapchain support, and queue family indices being filled out
			bool isDeviceSuitable(VkPhysicalDevice device);

			// Checks if the physical device has the extensions wanted by deviceExtensions
			bool checkDeviceExtensionSupport(VkPhysicalDevice device);

			// Creates the command pool that owns the memory command buffers record into,
			// destroying the pool frees every buffer allocated from it
			void createCommandPool();

			// Returns the max samples usable by the physical
			// device (VkPhysicalDeviceLimits)
			// Needed for multisampling
			VkSampleCountFlagBits getMaxUsableSampleCount();

		
	};
} // namespace Rath