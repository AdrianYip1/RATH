#include "application.hpp"

// Rath files
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
	setUpModels();
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
		updateScene();
		renderer->drawFrame(rScene);
	}
	renderer->wait();
} 

void Rath::Application::setUpModels() {
	rMaterial = std::make_unique<R_Material>();
	roomModel = std::make_unique<R_Model>();
	renderer->setUpModel(MODEL_PATH, TEXTURE_PATH, rMaterial.get(), roomModel.get());

	rCupMaterial = std::make_unique<R_Material>();
	cupModel = std::make_unique<R_Model>();
	renderer->setUpModel(MODEL2_PATH, TEXTURE2_PATH, rCupMaterial.get(), cupModel.get());
}

void Rath::Application::setUpScene() {
	R_SceneObject roomObject{};
	roomObject.model = roomModel.get();
	roomObject.baseTransform = enginemath::Mat4::rotateX(enginemath::toRad(-90.0f));
	roomObject.transform = roomObject.baseTransform;

	roomIndex = rScene.objects.size();
	rScene.objects.push_back(roomObject);

	R_SceneObject cupObject{};
	cupObject.model = cupModel.get();
	cupObject.transform = enginemath::Mat4::translationM(enginemath::Vec3(4.0f, 2.0f, 2.0f));
	cupObject.color = enginemath::Vec3(1.0f, 0.0f, 0.0f);

	rScene.objects.push_back(cupObject);

	// Light objects
	for (size i = 0; i < MAX_LIGHTS; i++) {
		R_SceneLight sceneLight{};
		sceneLight.model = cupModel.get();
		sceneLight.position = enginemath::Vec3(2.0f) * i;
		sceneLight.color = enginemath::Vec3(1.0f);
		rScene.lights.push_back(sceneLight);
	}
}

void Rath::Application::updateScene() {
	f32 t = camera.getElapsedTime();
	R_SceneObject& room = rScene.objects[roomIndex];
	room.transform = room.baseTransform * enginemath::Mat4::rotateX(std::sin(t));
}