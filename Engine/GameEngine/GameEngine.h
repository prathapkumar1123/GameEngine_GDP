#pragma once

#include "GLRenderer.h"
#include "Camera.h"
#include "Scene.h"

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

	void setCurrentScene(Scene* scene);

	Scene* getCurrentScene();

	GLRenderer* getRenderer();

	// Register functions for different game systems (e.g., physics, input, rendering).
	void registerUpdateFunction(std::function<void(float)> updateFunc);
	void registerRenderFunction(std::function<void()> renderFunc);

private:
	class GameEngineImpl;
	GameEngineImpl* impl;
};

