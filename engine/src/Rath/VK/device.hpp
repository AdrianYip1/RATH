#pragma once
#include <vulkan/vulkan.h>
#include <Rath/Core/defines.hpp>

namespace RATH {
	class Device {
	public:
		Device();
		~Device();
		Device(const Device& other) = delete;
		Device& operator=(const Device& other) = delete;

	private:

	};
} // namespace RATH