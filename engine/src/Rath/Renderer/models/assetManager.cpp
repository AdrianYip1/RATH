#include "assetManager.hpp"

Rath::R_AssetManager::R_AssetManager(Device& _device, Buffer& _buffer, Descriptor& _descriptor, Image& _image, Pipeline& _pipeline) :
	device(_device), buffer(_buffer), descriptor(_descriptor), image(_image), pipeline(_pipeline){

}

Rath::R_AssetManager::~R_AssetManager() {

}

Rath::R_Model* Rath::R_AssetManager::setUpModel(const std::string modelPath, const std::string texturePath) {
	std::string key = modelPath + "|" + texturePath;

	auto it = rModels.find(key);
	if (it != rModels.end()) {
		return it->second.get();
	}

	std::unique_ptr<R_Material> material = std::make_unique<R_Material>();
	std::unique_ptr<R_Model> model = std::make_unique<R_Model>();

	R_ModelMaterialCreateInfo rMaterialCreateInfo{};
	rMaterialCreateInfo.texturePath = texturePath;

	if (!R_Material::rCreateMaterial(device, buffer, descriptor, image, rMaterialCreateInfo, material.get())) {
		throw std::runtime_error("Failed to create material for model");
	}
	rMaterials.push_back(std::move(material));

	R_ModelCreateInfo rModelInfo{};
	rModelInfo.pipeline = pipeline.getGraphicsPipeline();
	rModelInfo.pipelineLayout = pipeline.getPipelineLayout();
	rModelInfo.material = rMaterials.back().get();
	rModelInfo.modelPath = modelPath;

	if (!R_Model::rCreateModel(device, buffer, rModelInfo, model.get())) {
		throw std::runtime_error("Failed to create model");
	}
	rModels[key] = std::move(model);
	return rModels[key].get();
}

