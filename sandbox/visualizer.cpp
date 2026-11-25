#include "visualizer.hpp"

Visualizer::Visualizer(const RATH::ApplicationConfig& config) :
	Application(config)
{
}

void Visualizer::onInit() {
	std::cout << "Visualizer application opened" << std::endl;
}

void Visualizer::onUpdate(RATH::f32 deltaTime) {

}

void Visualizer::onShutdown() {
	std::cout << "Visualizer application closed" << std::endl;
}
