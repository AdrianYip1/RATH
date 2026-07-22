// Contains physical/logical devices, queues and their indices

#pragma once
#include <vulkan/vulkan.h>
#include "Rath/Core/defines.hpp"

#include "context.hpp"

namespace Rath {
	class Device {
		public:
			VkDevice physicalDevice = VK_NULL_HANDLE;

			Device(Context& context);
			~Device();
			Device(const Device& other) = delete;
			Device& operator=(const Device& other) = delete;

		private:
			const Context& context;

			void pickPhysicalDevice();
			bool isDeviceSuitable();
			u32 findQueueFamilies(VkPhysicalDevice device);
	};
} // namespace Rath