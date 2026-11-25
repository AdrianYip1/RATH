#pragma once

#include <engine/app/application.hpp>

class Visualizer : public RATH::Application {
public:
	Visualizer(const RATH::ApplicationConfig& config);

protected:
	void onInit() override;
	void onUpdate(RATH::f32 deltaTime) override;
	void onShutdown() override;
};
