#include "vkPipeline.hpp"

RATH::Pipeline::Pipeline(Device& device, const GraphicsPipelineConfig& graphicsPipelineConfig) :
	_device(device), pipelineLayout(createPipelineLayout(device)),
	pipeline(createGraphicsPipeline(device, graphicsPipelineConfig, pipelineLayout))
{

};

RATH::Pipeline::~Pipeline() {
	vkDestroyPipeline(_device.getDevice(), pipeline, nullptr);
	vkDestroyPipelineLayout(_device.getDevice(), pipelineLayout, nullptr);
};


std::vector<char> RATH::Pipeline::readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("ERROR: vkPipeline.cpp -> readFile()");
	}

	size fileSize = (size)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

std::vector<VkShaderModule> RATH::Pipeline::createShaderModules(Device& device, const std::vector<std::vector<char>>& code) {
	std::vector<VkShaderModule> shaderModules(code.size());

	for(size i = 0; i < code.size(); i++) {
		VkShaderModuleCreateInfo shaderCreateInfo = {
			.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
			.codeSize = code[i].size(),
			.pCode = reinterpret_cast<const u32*>(code[i].data())
		};

		if (vkCreateShaderModule(device.getDevice(), &shaderCreateInfo, nullptr, &shaderModules[i]) != VK_SUCCESS) {
			throw std::runtime_error("ERROR: vkPipeline.cpp -> createShaderModules()");
		}
	}

	return shaderModules;
}

VkPipelineLayout RATH::Pipeline::createPipelineLayout(Device& device) {
	// Pipeline Layout
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
	};

	if (vkCreatePipelineLayout(device.getDevice(), &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkPipeline.cpp -> createPipelineLayout()");
	}

	return pipelineLayout;
}

VkPipeline RATH::Pipeline::createGraphicsPipeline(Device& device, const GraphicsPipelineConfig& config, VkPipelineLayout layout) {

	// Shader Config
	std::vector<std::vector<char>> shaderCode = {
		readFile(config.vertexShaderFilePath),
		readFile(config.fragmentShaderFilePath)
	};
	std::vector<VkShaderModule> shaderModules = createShaderModules(device, shaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_VERTEX_BIT,
		.module = shaderModules[0],
		.pName = "main"
	};
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		.stage = VK_SHADER_STAGE_FRAGMENT_BIT,
		.module = shaderModules[1],
		.pName = "main"
	};

	VkPipelineShaderStageCreateInfo shaderStagesCreateInfo[] = { vertShaderStageInfo, fragShaderStageInfo };

	// Vertex Input

	auto vertexBindingDescription = Vertex::getBindingDescription();
	auto vertexAttributeDescription = Vertex::getAttributeDescriptions();

	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
		.vertexBindingDescriptionCount = 1,
		.pVertexBindingDescriptions = &vertexBindingDescription,
		.vertexAttributeDescriptionCount = static_cast<u32>(vertexAttributeDescription.size()),
		.pVertexAttributeDescriptions = vertexAttributeDescription.data()
	};

	// Input Assembly
	VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
		.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		.primitiveRestartEnable = VK_FALSE
	};
	     
	// Viewport + Scissors (Dynamic)
	std::vector<VkDynamicState> dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
	};

	VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
		.dynamicStateCount = static_cast<u32>(dynamicStates.size()),
		.pDynamicStates = dynamicStates.data(),
	};

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
		.viewportCount = 1,
		.scissorCount = 1
	};

	// Rasterizer
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
		.depthClampEnable = VK_FALSE,
		.polygonMode = VK_POLYGON_MODE_FILL,
		.cullMode = VK_CULL_MODE_BACK_BIT,
		.frontFace = VK_FRONT_FACE_CLOCKWISE,
		.depthBiasEnable = VK_FALSE,
		.lineWidth = 1.0f
	};

	// Multisampling
	VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
		.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
		.sampleShadingEnable = VK_FALSE
	};

	// Depth + Stencil
	// do later, nullptr for now
	
	// Colour Blending
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {
		.blendEnable = VK_TRUE,
		.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
		.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		.colorBlendOp = VK_BLEND_OP_ADD,
		.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
		.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
		.alphaBlendOp = VK_BLEND_OP_ADD,
		.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT
	};

	VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
		.logicOpEnable = VK_FALSE,
		.attachmentCount = 1,
		.pAttachments = &colorBlendAttachment,
	};
	
	// Dynamic Rendering
	VkPipelineRenderingCreateInfo renderingCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
		.colorAttachmentCount = 1,
		.pColorAttachmentFormats = &config.colorFormat,
	};
	// Pipeline Creation
	VkPipeline graphicsPipeline = VK_NULL_HANDLE;

	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {
		.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
		.pNext = &renderingCreateInfo,
		.stageCount = 2,
		.pStages = shaderStagesCreateInfo,
		.pVertexInputState = &vertexInputCreateInfo,
		.pInputAssemblyState = &assemblyCreateInfo,
		.pViewportState = &viewportStateCreateInfo,
		.pRasterizationState = &rasterizerCreateInfo,
		.pMultisampleState = &multisamplingCreateInfo,
		.pDepthStencilState = nullptr,
		.pColorBlendState = &colorBlendStateCreateInfo,
		.pDynamicState = &dynamicStateCreateInfo,
		.layout = layout,
		.subpass = 0
		
	};

	if (vkCreateGraphicsPipelines(device.getDevice(), VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("ERROR: vkPipeline.cpp -> createGraphicsPipeline()");
	}

	for (VkShaderModule& shaderModule : shaderModules) {
		vkDestroyShaderModule(device.getDevice(), shaderModule, nullptr);
	}

	return graphicsPipeline;
}
