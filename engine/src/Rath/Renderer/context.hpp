// Contains instance, debug/validation, and surfaces
#pragma once
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <iostream>


#include "Rath/Core/defines.hpp"

namespace Rath {
	class Context {
		public:
			Context();
			~Context();
			Context(const Context& other) = delete;
			Context& operator=(const Context& other) = delete;

			void createInstance();

		private:
			VkInstance instance;
	};
} // namespace Rath