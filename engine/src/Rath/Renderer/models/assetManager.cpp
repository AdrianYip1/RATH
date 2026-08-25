#include "assetManager.hpp"

Rath::R_AssetManager::R_AssetManager(Device& _device, Buffer& _buffer, Descriptor& _descriptor, Image& _image, Pipeline& _pipeline) :
	device(_device), buffer(_buffer), descriptor(_descriptor), image(_image), pipeline(_pipeline){

}

Rath::R_AssetManager::~R_AssetManager() {

}

Rath::R_Model* Rath::R_AssetManager::setUpModel(const std::string modelPath, const std::string texturePath) {
	
	// Check for model dupes
	// pass in empty texturePath for gltf
	std::string key = modelPath + "|" + texturePath;

	auto it = rModels.find(key);
	if (it != rModels.end()) {
		return it->second.get();
	}

	std::unique_ptr<R_Model> model = std::make_unique<R_Model>();

	R_ModelCreateInfo rModelInfo{};
	rModelInfo.pipeline = pipeline.getGraphicsPipeline();
	rModelInfo.pipelineLayout = pipeline.getPipelineLayout();
	rModelInfo.modelPath = modelPath;

	if (!R_Model::rCreateModel(device, buffer, rModelInfo, model.get())) {
		throw std::runtime_error("Failed to create model");
	}


	for (auto& p : model->getPrimitives()) {
		std::string textureUri = (p.textureUri.empty() ? texturePath : p.textureUri);

		// Check for material dupes
		auto mIt = rMaterials.find(textureUri);
		if (mIt == rMaterials.end()) {
			std::unique_ptr<R_Material> material = std::make_unique<R_Material>();

			R_ModelMaterialCreateInfo rMaterialCreateInfo{};
			rMaterialCreateInfo.texturePath = textureUri;

			if (!R_Material::rCreateMaterial(device, buffer, descriptor, image, rMaterialCreateInfo, material.get())) {
				throw std::runtime_error("Failed to create material for model");
			}

			rMaterials[textureUri] = std::move(material);
		}
		p.material = rMaterials[textureUri].get();
	}

	rModels[key] = std::move(model);
	return rModels[key].get();
}

