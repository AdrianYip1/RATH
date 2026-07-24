#include "pipeline.hpp"

Rath::Pipeline::Pipeline(Device _device) : device(_device) {
	std::cout << "Created pipeline" << std::endl;
	createGraphicsPipeline();
}

Rath::Pipeline::~Pipeline() {
	std::cout << "Destroyed pipeline" << std::endl;

}

void Rath::Pipeline::createGraphicsPipeline() {
	auto vertShaderCode = readFile("../../shaders/basic.vert.spv");
	auto fragShaderCode = readFile("../../shaders/basic.frag.spv");

	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	vkDestroyShaderModule(device.getDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(device.getDevice(), vertShaderModule, nullptr);
}

VkShaderModule Rath::Pipeline::createShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const u32*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device.getDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module");
	}
}

std::vector<char> Rath::Pipeline::readFile(const std::string& fileName) {
	std::ifstream file(fileName, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file");
	}

	size fileSize = static_cast<size>(file.tellg());
	file.seekg(0);

	std::vector<char> buffer(fileSize);
	file.read(buffer.data(), fileSize);
	file.close();

	return buffer;
}