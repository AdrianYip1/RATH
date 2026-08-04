#pragma once
#include <vulkan/vulkan.h>

// Rath files
#include "Rath/Core/defines.hpp"
#include "../vertexData.hpp"

// std
#include <stdexcept>
#include <iostream>
#include <vector>
#include <cstring>
#include <array>

namespace Rath {
	class Model {
		public:
			Model();
			~Model();
			Model(const Model& other) = delete;
			Model& operator=(const Model& other) = delete;

			std::vector<Vertex> getVertices() const { return vertices; };
			std::vector<u32> getIndices() const { return indices; };

		private:
			std::vector<Vertex> vertices;
			std::vector<u32> indices;

			void loadModel();
	};
} // namespace Rath