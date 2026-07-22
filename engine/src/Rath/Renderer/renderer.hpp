// Contains frames in flight sync, command buffers,
// pools/sets, draw loop

#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>

#include "context.hpp"
#include "device.hpp"

#include "Rath/Platform/window.hpp"

namespace Rath {
	class Renderer {
		public:

			Renderer(Window& window);
			~Renderer();
			Renderer(const Renderer& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

		private:
			Context context;
			Device device;
	};
} // namespace Rath