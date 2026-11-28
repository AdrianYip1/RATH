// instances, devices, queues
#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "vkCheck.hpp"

struct GLFWwindow;

namespace RATH {

	struct QueueInfos {
		std::optional<u32> graphicsQueueIdx;
		std::optional<u32> presentQueueIdx;
		std::optional<u32> computeQueueIdx;
	};

	struct SwapchainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class Device {
	public :
		Device(GLFWwindow* window);
		~Device();
		Device(const Device& other) = delete;
		Device& operator=(const Device& other) = delete;

		VkSurfaceKHR getSurface() { return surface; };
		VkPhysicalDevice getPhysicalDevice() { return physicalDevice; };
		VkDevice getDevice() { return logicalDevice; };

		VkQueue getGraphicsQ() { return graphicsQueue; };
		VkQueue getPresentQ() { return presentQueue; };
		VkQueue getComputeQ() { return computeQueue; };

		// One-off commands
		VkCommandBuffer beginSingleTimeCommands();
		void endSingleTimeCommands(VkCommandBuffer commandBuffer);

		static SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
		static std::optional<RATH::QueueInfos> findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

	private:
		VkInstance instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;
		VkSurfaceKHR surface = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		QueueInfos queueInfos;
		VkDevice logicalDevice = VK_NULL_HANDLE;

		VkQueue graphicsQueue = VK_NULL_HANDLE;
		VkQueue presentQueue = VK_NULL_HANDLE;
		VkQueue computeQueue = VK_NULL_HANDLE;

		// used only for single time commands
		VkCommandPool transientCommandPool = VK_NULL_HANDLE;

		static VkInstance createInstance();
		static VkCommandPool createTransientCommandPool(VkDevice logicalDevice, u32 queueFamilyIdx);
		static VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance instance);
		
		static VkSurfaceKHR createSurface(VkInstance instance, GLFWwindow* window);
		static VkPhysicalDevice selectPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);
		static bool extensionsSuitable(VkPhysicalDevice device);
		static VkDevice selectLogicalDevice(VkPhysicalDevice physicalDevice, const QueueInfos& queueInfo);
	};
} // namespace RATH