#include "material.hpp"

// Rath
#include "../descriptor.hpp"

Rath::R_Material::~R_Material() {
	if (device == nullptr) return;
	vkDestroyDescriptorPool(device->getDevice(), materialDescriptorPool, nullptr);
}

bool Rath::R_Material::rCreateMaterial(Device& _device, Buffer& _buffer,
									  Descriptor& _descriptor, Image& _image,
									  const R_ModelMaterialCreateInfo& materialCreateInfo,
									  R_Material* _material) {
	if (_material == nullptr) return false;

	_material->device = &_device;
	_material->buffer = &_buffer;
	_material->descriptor = &_descriptor;
	_material->image = &_image;
	_material->pipeline = materialCreateInfo.pipeline;
	_material->texture = std::make_unique<Texture>(_device, _image, _buffer, materialCreateInfo.texturePath);
	

	_material->materialDescriptorPool = createDescriptorPool(_device, R_DESCRIPTOR_TYPE::R_SAMPLER, 1);
	_material->descriptorSet = _descriptor.createDescriptorSets(R_DESCRIPTOR_TYPE::R_SAMPLER, 
																_material->materialDescriptorPool,
																_descriptor.getSamplerLayout(),
																_material->texture.get())[0];
	return true;
}
