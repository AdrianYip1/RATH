// Contains frames in flight sync, command buffers,
// pools/sets, draw loop

#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>

#include "context.hpp"
#include "device.hpp"

namespace Rath {
	class Renderer {
		public:

			Renderer();
			~Renderer();
			Renderer(const Renderer& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;

		private:
			Context context;
			Device device;
	};
} // namespace Rath