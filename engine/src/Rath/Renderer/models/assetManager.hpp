#pragma once

// Rath
#include "../device.hpp"
#include "../descriptor.hpp"
#include "../buffers/buffer.hpp"
#include "../images/image.hpp"
#include "../pipeline.hpp"

#include "modelStruct.hpp"
#include "material.hpp"
#include "model.hpp"

// std
#include <vector>
#include <memory>


namespace Rath {
	class R_AssetManager {
		public:
			R_AssetManager(Device& _device, Buffer& _buffer, Descriptor& _descriptor, Image& _image, Pipeline& _pipeline);
			~R_AssetManager();

			// called by application.cpp
			R_Model* setUpModel(const std::string modelPath, const std::string texturePath);

		private:
			Device& device;
			Buffer& buffer;
			Descriptor& descriptor;
			Image& image;
			Pipeline& pipeline;

			std::vector<std::unique_ptr<R_Model>> rModels;
			std::vector<std::unique_ptr<R_Material>> rMaterials;
	};
} // namespace Rath