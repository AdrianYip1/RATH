#include "application.hpp"

// Rath files
#include "Rath/Renderer/models/assetManager.hpp"

#include "Rath/Renderer/renderer.hpp"
#include "Rath/Renderer/context.hpp"
#include "Rath/Renderer/device.hpp"
#include "Rath/Renderer/buffers/buffer.hpp"

#include "Rath/Renderer/models/material.hpp"
#include "Rath/Renderer/models/modelStruct.hpp"
#include "Rath/Renderer/models/model.hpp"

// Application Constructor
Rath::Application::Application(u32 width, u32 height, 
	const char* title) : 
	window(width, height, title),
	input(window), camera(enginemath::Vec3(0.0f, 0.5f, 4.0f), width / (f32)height), 
	cameraController(window, input, camera),
	context(std::make_unique<Context>(window)),
	device(std::make_unique<Device>(*context)),
	buffer(std::make_unique<Buffer>(*device)),
	renderer(std::make_unique<Renderer>(window, camera, *context, *device, *buffer)) 
{
	setUpScene();
}

// Application Destructor
Rath::Application::~Application() { 

}

// Entrypoint to the application -> calls initVulkan()
// then runs mainLoop()
void Rath::Application::run() {
	// initVulkan();
	mainLoop();
}

// Main loop which polls for window resize/close/minimize
// and starts the drawing loop
// Has delta time which gets threaded down to uniform
void Rath::Application::mainLoop() {
	// Init the clock + last time point
	auto lastTime = std::chrono::high_resolution_clock::now();
	while (!window.shouldClose()) {
		// Delta time
		auto currentTime = std::chrono::high_resolution_clock::now();

		f32 deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - lastTime).count();
		lastTime = currentTime;
		camera.setDeltaTime(deltaTime);

		window.pollEvents();
		// Accumulate controls before updating camera
		cameraController.checkCameraMovement();
		cameraController.checkMouse();
		cameraController.updateCamera();

		// check for any pending spawns/removals
		if (!rScene.pendingSpawns.empty()) {
			handlePendingSpawns();
		}
		if (!rScene.pendingRemovals.empty()) {
			handlePendingRemovals();
		}

		updateScene();
		renderer->drawFrame(rScene);
	}
	renderer->wait();
} 

void Rath::Application::setUpScene() {
	R_SceneObject roomObject{};
	roomObject.model = renderer->loadModel(MODEL_PATH, TEXTURE_PATH);
	roomObject.baseTransform = enginemath::Mat4::rotateX(enginemath::toRad(-90.0f));
	roomObject.transform = roomObject.baseTransform;
	roomObject.id = objectIndex;

	rScene.objects[objectIndex++] = roomObject;

	R_SceneObject cupObject{};
	cupObject.model = renderer->loadModel(MODEL2_PATH, TEXTURE2_PATH);
	cupObject.transform = enginemath::Mat4::translationM(enginemath::Vec3(4.0f, 2.0f, 2.0f));
	cupObject.color = enginemath::Vec3(1.0f, 0.0f, 0.0f);
	cupObject.id = objectIndex;

	rScene.objects[objectIndex++] = cupObject;

	// Light objects
	for (size i = 0; i < MAX_LIGHTS; i++) {
		R_SceneLight sceneLight{};
		sceneLight.model = renderer->loadModel(MODEL2_PATH, TEXTURE2_PATH);
		sceneLight.position = enginemath::Vec3(2.0f) * i;
		sceneLight.color = enginemath::Vec3(1.0f);
		sceneLight.id = lightIndex;
		rScene.lights[lightIndex++] = sceneLight;
	}
}

void Rath::Application::updateScene() {
	f32 t = camera.getElapsedTime();
	auto it = rScene.objects.find(0);
	if (it == rScene.objects.end()) return;
	it->second.transform = it->second.baseTransform * enginemath::Mat4::rotateX(std::sin(t));
}


void Rath::Application::handlePendingSpawns() {
	for (auto& spawn : rScene.pendingSpawns) {
		if (spawn.type == Rath::R_SCENE_TYPE::R_SCENE_TYPE_OBJECT) {
			R_SceneObject newObject{};

			newObject.model = renderer->loadModel(spawn.modelPath, spawn.texturePath);
			newObject.baseTransform = spawn.baseTransform;
			newObject.transform = spawn.transform;
			newObject.id = objectIndex;

			rScene.objects[objectIndex++] = newObject;
		}
		else {
			R_SceneLight sceneLight{};
			sceneLight.model = renderer->loadModel(spawn.modelPath, spawn.texturePath);
			sceneLight.position = enginemath::Vec3(2.0f) * lightIndex;
			sceneLight.color = enginemath::Vec3(1.0f);
			sceneLight.id = lightIndex;

			rScene.lights[lightIndex++] = sceneLight;
		}
	}
	rScene.pendingSpawns.clear();
	return;
}

void Rath::Application::handlePendingRemovals() {
	for (auto& removal : rScene.pendingRemovals) {
		if (removal.type == Rath::R_SCENE_TYPE::R_SCENE_TYPE_OBJECT) {
			rScene.objects.erase(removal.id);
		}
		else {
			rScene.lights.erase(removal.id);
		}
	}
	rScene.pendingRemovals.clear();
	return;
}