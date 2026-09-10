#pragma once
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <Rath/Core/defines.hpp>

namespace RATH{
	class Context {
	public:
		Context();
		~Context();
		Context(const Context& other) = delete;
		Context& operator=(const Context& other) = delete;

	private:
		void createInstance();

		VkInstance instance = VK_NULL_HANDLE;
	};
} // namespace RATH