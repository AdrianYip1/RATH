#include "ui.hpp"

Rath::UI::UI(Window& _window, Context& _context, Device& _device, 
	Renderpass& _renderpass, Swapchain& _swapchain, Pipeline& _pipeline) : 
	window(_window), context(_context), device(_device), 
	renderpass(_renderpass), swapchain(_swapchain), pipeline(_pipeline) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight()

	// Setup platform/renderer backends
	// when true is passed, imgui registers its input functions for this window
	// meaning GLFW calls imgui's functions when something happens
	ImGui_ImplGlfw_InitForVulkan(window.getWindow(), true);
	ImGui_ImplVulkan_InitInfo initInfo{};
	initInfo.ApiVersion = VK_API_VERSION_1_0;
	initInfo.Instance = context.getInstance();
	initInfo.PhysicalDevice = device.getPhysicalDevice();
	initInfo.Device = device.getDevice();
	initInfo.Queue = device.getGraphicsQueue();
	// Get the index of the queueFamily 
	initInfo.QueueFamily = device.findQueueFamilies(device.getPhysicalDevice()).graphicsAndComputeFamily.value();
	initInfo.MinImageCount = 2;
	initInfo.ImageCount = swapchain.getImageCount();
	initInfo.DescriptorPoolSize = 8;
	initInfo.PipelineInfoMain.RenderPass = renderpass.getRenderPass();
	initInfo.PipelineInfoMain.MSAASamples = device.getMSAASampleCount();
	ImGui_ImplVulkan_Init(&initInfo);
}

Rath::UI::~UI() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Rath::UI::startFrame() {
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Rath::UI::drawUI() {
	// Creating a window called Hello, and append into it
	ImGui::Begin("UI");

	ImGui::SliderInt("Number of rooms", &NUMBER_OF_ROOMS, 1, 5);
	ImGui::End();
	ImGui::Render();
}

void Rath::UI::draw(VkCommandBuffer commandBuffer) {
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}