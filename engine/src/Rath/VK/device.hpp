#pragma once
#include <vulkan/vulkan.h>
#include <Rath/Core/defines.hpp>

#include "../Platform/window.hpp"

namespace RATH {
	class Device {
	public:
		Device(Window& _window);
		~Device();
		Device(const Device& other) = delete;
		Device& operator=(const Device& other) = delete;

	private:
		Window& window;
	};
} // namespace RATH