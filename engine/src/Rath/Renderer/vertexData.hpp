#pragma once
#include "vulkan/vulkan.h"
#include "enginemath/vec2.hpp"
#include "enginemath/vec3.hpp"
#include "enginemath/mat4.hpp"

// std
#include <array>
#include <functional>

namespace Rath {
	// Push constant struct (move later)
	struct MeshPushConstant {
		enginemath::Mat4 model;
		alignas(16) enginemath::Vec3 color;
	};

	// Vertex struct: holds info about pos, color, texCoords
	// Contains helpers to set binding + attribute descriptions
	struct Vertex {
		enginemath::Vec3 pos;
		enginemath::Vec3 color;
		enginemath::Vec2 texCoord;
		enginemath::Vec3 normal;

		static VkVertexInputBindingDescription getBindingDescription() {
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(Vertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			return bindingDescription;
		}

		static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
			std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions{};
			attributeDescriptions[0].binding = 0;
			attributeDescriptions[0].location = 0;
			attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[0].offset = offsetof(Vertex, pos);

			attributeDescriptions[1].binding = 0;
			attributeDescriptions[1].location = 1;
			attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[1].offset = offsetof(Vertex, color);

			attributeDescriptions[2].binding = 0;
			attributeDescriptions[2].location = 2;
			attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescriptions[2].offset = offsetof(Vertex, texCoord);

			attributeDescriptions[3].binding = 0;
			attributeDescriptions[3].location = 3;
			attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
			attributeDescriptions[3].offset = offsetof(Vertex, normal);

			return attributeDescriptions;
		}

		bool operator==(const Vertex& other) const {
			return normal == other.normal && 
				   pos == other.pos && 
				   color == other.color && 
				   texCoord == other.texCoord;
		}
	};
} // namespace Rath

// Hash for Vertex
namespace std {
	template<> struct hash<Rath::Vertex> {
		size_t operator()(Rath::Vertex const& vertex) const {
			size_t seed = 0;
			auto combine = [&seed](float value) {
				seed ^= std::hash<float>()(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
				};

			combine(vertex.pos.x);
			combine(vertex.pos.y);
			combine(vertex.pos.z);
			combine(vertex.color.x);
			combine(vertex.color.y);
			combine(vertex.color.z);
			combine(vertex.texCoord.x);
			combine(vertex.texCoord.y);

			return seed;
		}
	};
} // namespace std