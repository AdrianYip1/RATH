#pragma once

#include <engine/core/defines.hpp>
#include <engine/core/stdIncludes.hpp>

namespace RATH {
	class Window;
	class Device;
	class Swapchain;
	class Pipeline;
	class Render;
	class Buffer;

	struct ApplicationConfig {
		const char* title = "RATH";
		u32 width = 1280;
		u32 height = 720;
	};

	class Application {
	public:
		Application(const ApplicationConfig& config);
		virtual ~Application();
		Application(const Application& other) = delete;
		Application& operator=(const Application& other) = delete;

		void run();

	protected:
		// Overridden by the game
		virtual void onInit() {}
		virtual void onUpdate(f32 deltaTime) {}
		virtual void onShutdown() {}
		
	private:
		std::unique_ptr<Window> window;
		std::unique_ptr<Device> device;
		std::unique_ptr<Swapchain> swapchain;
		std::unique_ptr<Pipeline> pipeline;
		std::unique_ptr<Render> render;
		std::vector<std::unique_ptr<Buffer>> vertexBuffers;
		std::vector<std::unique_ptr<Buffer>> indexBuffers;
	};
} // namespace RATH
