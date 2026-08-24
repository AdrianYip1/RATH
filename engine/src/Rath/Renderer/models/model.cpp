#include "model.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#include "vendor/tiny_obj_loader.h"
#define CGLTF_IMPLEMENTATION
#include "vendor/cgltf.h"

// std
#include <unordered_map>

// Model destructor
Rath::R_Model::~R_Model() {
	if (device == nullptr) return;

	vkDestroyBuffer(device->getDevice(), indexBuffer, nullptr);
	vkFreeMemory(device->getDevice(), indexBufferMemory, nullptr);

	vkDestroyBuffer(device->getDevice(), vertexBuffer, nullptr);
	vkFreeMemory(device->getDevice(), vertexBufferMemory, nullptr);
}

bool Rath::R_Model::rCreateModel(Device& _device, Buffer& _buffer,
				  const R_ModelCreateInfo& info, R_Model* _model) {
	// Needs a value model to fill information about
	if (_model == nullptr) return false;

	_model->device = &_device;
	_model->buffer = &_buffer;
	_model->modelPath = info.modelPath;
	_model->material = info.material;
	_model->pipeline = info.pipeline;
	_model->pipelineLayout = info.pipelineLayout;

	_model->loadModel();

	_model->createVertexBuffer();
	_model->createIndexBuffer();
	
	return true;
}

void Rath::R_Model::handleNodes(cgltf_node* node) {
	// If end of the recursive call, nothing will happen
	if (node->mesh) {
		f32 m[16];
		cgltf_node_transform_world(node, m);

		enginemath::Mat4 worldTransform(
			enginemath::Vec4(m[0], m[1], m[2], m[3]),
			enginemath::Vec4(m[4], m[5], m[6], m[7]),
			enginemath::Vec4(m[8], m[9], m[10], m[11]),
			enginemath::Vec4(m[12], m[13], m[14], m[15])
		);

		for (size i = 0; i < node->mesh->primitives_count; i++) {
			Vertex vertex{};
			cgltf_primitive* prim = &node->mesh->primitives[i];

			cgltf_accessor* posAccessor = nullptr;
			cgltf_accessor* normalAccessor = nullptr;
			cgltf_accessor* texCoordAccessor = nullptr;
			cgltf_accessor* colorAccessor = nullptr;

			for (size a = 0; a < prim->attributes_count; a++) {
				cgltf_attribute* attrib = &prim->attributes[a];

				if (attrib->type == cgltf_attribute_type_position) {
					posAccessor = attrib->data;
					continue;
				}
				else if (attrib->type == cgltf_attribute_type_normal) {
					normalAccessor = attrib->data;
					continue;
				}
				else if (attrib->type == cgltf_attribute_type_color) {
					colorAccessor = attrib->data;
					continue;
				}
				else if (attrib->type == cgltf_attribute_type_texcoord) {
					texCoordAccessor = attrib->data;
					continue;
				}
			}

			// number of vertices in the primitive since position's
			// accessor's count has one position per vertex
			cgltf_size n = posAccessor->count;

			// basically the vertices and indices of all the
			// primitives are being compacted into one vector
			// so the offset is needed to keep track of which
			// indices are drawn for each primitive -> adding vertices size after one has been pushed
			// to ensure the next primitive's indices correspond
			// to the location they are being pushed into
			u32 vertexOffset = static_cast<u32>(vertices.size());

			// loop for every vertex
			for (size s = 0; s < n; s++) {
				cgltf_accessor_read_float(posAccessor, s, vertex.pos.elements, 3);
				if (normalAccessor) cgltf_accessor_read_float(normalAccessor, s, vertex.normal.elements, 3);
				if (texCoordAccessor) cgltf_accessor_read_float(texCoordAccessor, s, vertex.texCoord.data, 2);
				vertex.color = enginemath::Vec3(1.0f);

				enginemath::Vec4 p = worldTransform * enginemath::Vec4::toVec4Pos(vertex.pos);
				vertex.pos = { p.x, p.y, p.z };

				enginemath::Vec4 norm = worldTransform * enginemath::Vec4::toVec4Dir(vertex.normal);
				vertex.normal = { norm.x, norm.y, norm.z };

				vertices.push_back(vertex);
			}

			// indices
			for (cgltf_size index = 0; index < prim->indices->count; index++) {
				cgltf_size idx = cgltf_accessor_read_index(prim->indices, index);
				indices.push_back(vertexOffset + static_cast<u32>(idx));
			}
		}
	}

	// recursive call to handle every node in the scene
	for (size i = 0; i < node->children_count; i++) {
		handleNodes(node->children[i]);
	}
}

void Rath::R_Model::loadModel() {
	cgltf_options options = {};
	cgltf_data* data = NULL;

	if (cgltf_parse_file(&options, modelPath.c_str(), &data) != cgltf_result_success) {
		throw std::runtime_error("Failed to parse gltf");
	};

	if (cgltf_load_buffers(&options, data, modelPath.c_str()) != cgltf_result_success) {
		throw std::runtime_error("Failed to load gltf buffers (is the .bin next to the .gltf?)");
	}

	for (cgltf_size i = 0; i < data->scene->nodes_count; i++) {
		handleNodes(data->scene->nodes[i]);
	}

	cgltf_free(data);
	//tinyobj::attrib_t attrib;
	//std::vector<tinyobj::shape_t> shapes;
	//std::vector<tinyobj::material_t> materials;
	//std::string err;
	//std::string warn;

	//if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath.c_str())) {
		//throw std::runtime_error(err);
	//}
}

// Wrapper for creating the vertex buffer
// Uses a staging buffer first before copying vertices data into the actual
// vertexBuffer (which holds the GPU optimized memory) -> staging buffer is destroyed after
// Both the staging and vertex buffers call the createBuffer method
void Rath::R_Model::createVertexBuffer() {
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size)bufferSize);
	vkUnmapMemory(device->getDevice(), stagingBufferMemory);

	buffer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer, vertexBufferMemory);

	buffer->copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);
}

// Wrapper for creating the index buffer
// Staging buffer used here before copying indices data into
// indexBuffer (which holds the GPU optimized memory) -> staging buffer destroyed after
// Both the staging and indices buffers call createBuffer method
void Rath::R_Model::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	buffer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
		stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device->getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size)bufferSize);
	vkUnmapMemory(device->getDevice(), stagingBufferMemory);

	buffer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer, indexBufferMemory);
	buffer->copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device->getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(device->getDevice(), stagingBufferMemory, nullptr);

}

void Rath::R_Model::bind(VkCommandBuffer commandBuffer) {
	VkBuffer vertexBuffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

}

void Rath::R_Model::draw(VkCommandBuffer commandBuffer) {
	vkCmdDrawIndexed(commandBuffer, static_cast<u32>(indices.size()), 1, 0, 0, 0);
}

// Binds sets 1 and 2 later on
// Set 0: per frame
// Set 1: per material (shared by every object using this material)
// Set 2: per object
void Rath::R_Model::bindDescriptors(VkCommandBuffer commandBuffer) {
	VkDescriptorSet set = material->getDescriptorSet();
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 1, 1, &set, 0, nullptr);
}

void Rath::R_Model::bindPipeline(VkCommandBuffer commandBuffer) {
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}