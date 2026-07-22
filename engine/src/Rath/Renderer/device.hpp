// Contains physical/logical devices, queues and their indices

#pragma once
#include <vulkan/vulkan.h>
#include "Rath/Core/defines.hpp"
#include <map>
#include <optional>

#include "context.hpp"

namespace Rath {
	struct QueueFamilyIndices {
		std::optional<u32> graphicsFamily;

		bool isComplete() {
			return graphicsFamily.has_value();
		}
	};

	class Device {
		public:
			Device(Context& context);
			~Device();
			Device(const Device& other) = delete;
			Device& operator=(const Device& other) = delete;

		private:
			const Context& context;
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
			VkDevice device = VK_NULL_HANDLE;
			// Retrieve queue handles using vkGetDeviceQueue
			VkQueue graphicsQueue = VK_NULL_HANDLE;

			void pickPhysicalDevice();
			void createLogicalDevice();
			bool isDeviceSuitable(VkPhysicalDevice device);
			QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	};
} // namespace Rath