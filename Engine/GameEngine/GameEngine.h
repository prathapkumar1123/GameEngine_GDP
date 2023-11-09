#pragma once

#include "GLRenderer.h"
#include "Camera.h"
#include "BaseScene.h"

#include <functional>

class GameEngine {
public:

	GameEngine();
	~GameEngine();

	void init();
	void release();
	void runGameLoop();
	void stop();

	GLFWwindow* getWindow();

	Camera& camera = Camera::getInstance();

	LightManager* lightManager = nullptr;

	int getShaderProgramId();

	void setAssetsPath(std::string modelsBasePath, std::string shadersBasePath, std::string scenesBasePath);

	void setCurrentScene(BaseScene* scene);

	BaseScene* getCurrentScene();
	 
	GLRenderer* getRenderer();

	VAOManager* getVAOManager();

	void setFPSRate(FPS_RATE fpsRate);

	// Register functions for different game systems (e.g., physics, input, rendering).
	void registerUpdateFunction(std::function<void(float)> updateFunc);
	void registerRenderFunction(std::function<void()> renderFunc);

private:
	class GameEngineImpl;
	GameEngineImpl* impl;
};

