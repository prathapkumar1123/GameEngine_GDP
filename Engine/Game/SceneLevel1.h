#pragma once

#include "Ball.h"
#include "../GameEngine/BaseScene.h"
#include "../Graphics/VAOManager/VAOManager.h";

class SceneLevel1 : public BaseScene {
public:
    SceneLevel1(int shaderProgramId, std::string sceneFile);

    // Implement the pure virtual functions from the base class
    void onInit() override;
    void onSceneLoaded() override;
    void updateScene(float deltaTime) override;
    void processKeyboardInput(float deltaTime) override;

private:
    Ball* playerBall = nullptr;
    void buildSpheres();
};