#pragma once

#include <vulkan/vulkan.h>
#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

#include "vkDevice.hpp"

#include "vkVertex.hpp"

#include "RATH_VULKAN_RHI_MACROS.hpp"

namespace RATH {
	struct GraphicsPipelineConfig {
		std::string vertexShaderFilePath;
		std::string fragmentShaderFilePath;
		VkFormat colorFormat = VK_FORMAT_UNDEFINED;
	};

	class Pipeline {
	public:
		Pipeline(Device& device, const GraphicsPipelineConfig& graphicsPipelineConfig);
		// have overloads for other pipelines
		~Pipeline();
		Pipeline(const Pipeline& other) = delete;
		Pipeline& operator=(const Pipeline& other) = delete;

		VkPipelineLayout getPipelineLayout() { return pipelineLayout; };
		VkPipeline getPipeline() { return pipeline; };

	private:
		Device& _device;
		VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
		VkPipeline pipeline = VK_NULL_HANDLE;

		static std::vector<char> readFile(const std::string& fileName);
		static std::vector<VkShaderModule> createShaderModules(Device& device, const std::vector<std::vector<char>>& code);

		static VkPipelineLayout createPipelineLayout(Device& device);
		static VkPipeline createGraphicsPipeline(Device& device, const GraphicsPipelineConfig& config, VkPipelineLayout layout);
	};
} // namespace RATH