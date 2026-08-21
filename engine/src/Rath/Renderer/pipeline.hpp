#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "vertexData.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
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

			// Returns graphicsPipeline
			VkPipeline getGraphicsPipeline() const { return graphicsPipeline; };

			// Returns pipelineLayout
			VkPipelineLayout getPipelineLayout() const { return pipelineLayout; };

		private:
			Device& device;
			Swapchain& swapchain;
			Renderpass& renderpass;
			Descriptor& descriptor;

			VkPipelineLayout pipelineLayout;
			VkPipeline graphicsPipeline;

			// Builds every create info the pipeline needs, then creates the layout
			// and the pipeline. Viewport and scissor are dynamic, set at record time
			void createGraphicsPipeline();

			// Takes the byte data of a shader and returns a corresponding shader module
			VkShaderModule createShaderModule(const std::vector<char>& code);

			// Reads the file at the given path and returns its byte data,
			// read in binary with the size computed from opening the file at the end
			static std::vector<char> readFile(const std::string& fileName);
	};
} // namespace Rath