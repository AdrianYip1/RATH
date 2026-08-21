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

// ImGuiCond_FirstUseEver -> sets inital size/position and then you can drag freely after
// ImGuiCond_Always -> force window size and pos every frame
void Rath::UI::drawUI(R_Scene& rScene) {
	// Get the viewwport size to scale
	const ImGuiViewport* vp = ImGui::GetMainViewport();

	// Creating a window and append into it
	ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x, vp->WorkPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x * 0.25, vp->WorkSize.y * 0.25), ImGuiCond_Always);
	ImGui::Begin("UI", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
	//ImGui::DragFloat3("Light position", &rScene.lights[0].position.x, 0.25f, -5.0f, 5.0f);

	for (auto& [id, light] : rScene.lights) {
		ImGui::PushID(id);
		ImGui::DragFloat3("Position", &light.position[0], 0.25, -5.0f, 5.0f);
		ImGui::PopID();

		std::string label = "Delete Light " + std::to_string(id);
		if (ImGui::Button(label.c_str())) {
			PendingRemoval removal{};
			removal.id = id;
			removal.type = Rath::R_SCENE_TYPE::R_SCENE_TYPE_LIGHT;

			rScene.pendingRemovals.push_back(removal);
		}
	}

	ImGui::End();

	// Create another window
	ImGui::SetNextWindowPos(ImVec2(vp->WorkPos.x + vp->WorkSize.x, vp->WorkPos.y), ImGuiCond_Always, ImVec2(1, 0));
	ImGui::SetNextWindowSize(ImVec2(vp->WorkSize.x * 0.25, vp->WorkSize.y * 0.25), ImGuiCond_Always);
	ImGui::Begin("Add Mesh Button");
	// Add mesh should open another menu to select mesh properties and types
	static bool addMeshMenu = false;
	if (ImGui::Button("Add mesh")) {
		addMeshMenu = true;
	}
	if (addMeshMenu) {
		static PendingSpawn type{};
		ImGui::DragFloat3("Choose Model Location", &type.position[0], 0.25, -5.0f, 5.0f);
		ImGui::ColorEdit3("Choose Model Color", &type.color[0]);

		// combo box for type of scene object being spawned
		const char* sceneTypes[] = { "Mesh", "Light" };
		const std::vector<R_SCENE_TYPE> rSceneTypes = { R_SCENE_TYPE::R_SCENE_TYPE_OBJECT, R_SCENE_TYPE::R_SCENE_TYPE_LIGHT };
		static const char* currentItem = sceneTypes[0];

		if (ImGui::BeginCombo("Object Type", currentItem)) {
			for (int n = 0; n < IM_ARRAYSIZE(sceneTypes); n++) {
				bool isSelected = (currentItem == sceneTypes[n]);

				if (ImGui::Selectable(sceneTypes[n], isSelected)) {
					currentItem = sceneTypes[n];
					type.type = rSceneTypes[n];
				}
				if (isSelected) {
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
		if (ImGui::Button("Spawn with selected values")) {
			rScene.pendingSpawns.push_back(type);
			addMeshMenu = false;
		}

	}

	for (auto& [id, model] : rScene.objects) {
		// Every model should be movable and editable
		std::string modelLabel = "Update Model " + std::to_string(id);
		ImGui::Text(modelLabel.c_str());

		// push and pop id because the dragfloat3 shares the same "Position" label for each model
		// make them independent by making an internal id hashing label + id
		ImGui::PushID(id);
		ImGui::DragFloat3("Position", &model.position[0], 0.25, -5.0f, 5.0f);
		ImGui::PopID();

		std::string deleteLabel = "Delete Model " + std::to_string(id);
		if (ImGui::Button(deleteLabel.c_str())) {
			PendingRemoval removal{};
			removal.id = id;
			removal.type = Rath::R_SCENE_TYPE::R_SCENE_TYPE_OBJECT;

			rScene.pendingRemovals.push_back(removal);
		}
	}
	ImGui::End();


	ImGui::Render();
}

void Rath::UI::draw(VkCommandBuffer commandBuffer) {
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
}