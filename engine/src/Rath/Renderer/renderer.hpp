// Contains frames in flight sync, command buffers,
// pools/sets, draw loop
#pragma once
#include <vulkan/vulkan.h>
#include "enginemath/mat4.hpp"
#include "enginemath/vec3.hpp"

// Rath files
#include "context.hpp"
#include "device.hpp"
#include "swapchain.hpp"
#include "renderpass.hpp"
#include "pipeline.hpp"
#include "buffers/buffer.hpp"
#include "buffers/uniform.hpp"
#include "images/image.hpp"
#include "descriptor.hpp"
#include "images/texture.hpp"
#include "images/color.hpp"
#include "buffers/storage.hpp"
#include "images/depth.hpp"
#include "models/model.hpp"
#include "Rath/Platform/window.hpp"
#include "Rath/Core/camera/camera.hpp"
#include "models/material.hpp"
#include "models/modelStruct.hpp"
#include "ui/ui.hpp"
#include "scenes/light.hpp"
#include "models/assetManager.hpp"

// std
#include <stdexcept>
#include <memory>


namespace Rath {
	class Renderer {

		public:

			Renderer(Window& _window, Camera& _camera, Context& _context, Device& _device, Buffer& _buffer);
			~Renderer();
			Renderer(const Renderer& other) = delete;
			Renderer& operator=(const Renderer& other) = delete;
			
			// Draws one frame: waits on the fence, acquires an image, updates the
			// uniform buffer, records and submits, then presents
			void drawFrame(R_Scene& rScene);

			// Waits for all queues to finish, needed before destruction
			// can't call vkDeviceWaitIdle(device) in application so have a function here
			void wait();

			R_Model* loadModel(const std::string modelPath, const std::string texturePath);

		private:
			Window& window;
			Camera& camera;
			Context& context;
			Device& device;
			Buffer& buffer;

			Swapchain swapchain;
			Image image;
			Color color;
			Depth depth;
			Renderpass renderpass;
			UniformBuffer uniformBuffer;
			R_Light light;
			Storage storage;
			Descriptor descriptor;
			Pipeline pipeline;
			R_AssetManager assetManager;
			UI ui;

			// RATH members
			R_Material rMaterial;
			R_Material rCupMaterial;

			std::vector<VkCommandBuffer> commandBuffers;
			std::vector<VkCommandBuffer> computeCommandBuffers;

			std::vector<VkSemaphore> imageAvailableSemaphores;
			std::vector<VkSemaphore> renderFinishedSemaphores;
			std::vector<VkFence> inFlightFences;

			// Sync objects for compute
			std::vector<VkSemaphore> computeFinishedSemaphores;
			std::vector<VkFence> computeInFlightFences;

			u32 currentFrame = 0;

			// Allocates a command buffer per frame in flight from the command pool,
			// freed when the command pool is destroyed
			void createCommandBuffers();

			// Creates the semaphores and fences needed for rendering
			void createSyncObjects();

			// Begins recording, starts the render pass, binds the pipeline, sets the
			// dynamic viewport and scissor, then binds the vertex, descriptor and
			// index data before drawing
			void recordCommandBuffer(VkCommandBuffer commandBuffer, u32 imageIndex, R_Scene& rScene);
			
			// Begins a command buffer (compute), binds compute descriptor set, then dispatches
			void recordComputeCommandBuffer(VkCommandBuffer commandBuffer);
			
	};
} // namespace Rath