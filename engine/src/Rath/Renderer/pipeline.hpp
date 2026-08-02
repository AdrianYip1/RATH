#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "buffers/vertex.hpp"
#include "descriptor.hpp"
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
			Pipeline(Device& _device, Swapchain& _swapchain, 
				     Renderpass& _renderpass, Descriptor& _descriptor);
			~Pipeline();
			Pipeline(const Pipeline& other) = delete;
			Pipeline& operator=(const Pipeline& other) = delete;

			VkPipeline getGraphicsPipeline() const { return graphicsPipeline; };
			VkPipelineLayout getPipelineLayout() const { return pipelineLayout; };
		
		private:
			Device& device;
			Swapchain& swapchain;
			Renderpass& renderpass;
			Descriptor& descriptor;

			VkPipelineLayout pipelineLayout;
			VkPipeline graphicsPipeline;

			void createGraphicsPipeline();
			VkShaderModule createShaderModule(const std::vector<char>& code);
			static std::vector<char> readFile(const std::string& fileName);
	};
} // namespace Rath