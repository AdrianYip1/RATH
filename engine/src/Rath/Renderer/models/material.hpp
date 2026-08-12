#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"
#include "../device.hpp"
#include "../buffers/buffer.hpp"
#include "../images/texture.hpp"
#include "../images/image.hpp"



// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>
#include <unordered_map>

namespace Rath{
	class Pipeline;
	class Descriptor;

	// Structure for how a model looks
	// Textures and pipeline for now
	struct R_ModelMaterialCreateInfo {
		Pipeline* pipeline;
		std::string texturePath; // optional
	};

	class R_Material {
		public:
			R_Material() = default;
			~R_Material();
			R_Material(const R_Material& other) = delete;
			R_Material& operator=(const R_Material& other) = delete;

			static bool rCreateMaterial(Device& _device, Buffer& _buffer, 
										Descriptor& _descriptor, Image& _image,
										R_ModelMaterialCreateInfo& materialCreateInfo,
										R_Material* _material);

			VkDescriptorSet getDescriptorSet() const { return descriptorSet; }

		private:
			Device* device;
			Buffer* buffer;
			Descriptor* descriptor;
			Image* image;
			Pipeline* pipeline;
			std::unique_ptr<Texture> texture;
			VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
			VkDescriptorPool materialDescriptorPool = VK_NULL_HANDLE;

	};

} // namespace Rath