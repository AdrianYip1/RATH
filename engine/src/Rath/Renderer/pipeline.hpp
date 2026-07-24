#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "device.hpp"
#include "Rath/Core/defines.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <map>
#include <set>
#include <optional>
#include <string>
#include <fstream>
#include <vector>

namespace Rath {
	class Pipeline {
		public:
			Pipeline(Device _device);
			~Pipeline();
			Pipeline(const Pipeline& other) = delete;
			Pipeline& operator=(const Pipeline& other) = delete;

		private:
			Device& device;

			void createGraphicsPipeline();
			VkShaderModule createShaderModule(const std::vector<char>& code);
			static std::vector<char> readFile(const std::string& fileName);
	};
} // namespace Rath