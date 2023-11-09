#pragma once

#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/LightsManager/LightManager.h"
#include "../Graphics/VAOManager/VAOManager.h"
#include "../GameEngine/Camera.h"

#include <iostream>
#include <string>
#include <vector>

class BaseScene {
public:
    BaseScene(int& shaderProgramId, std::string sceneFile);
    ~BaseScene();

    std::string sceneFile;

    virtual void onInit() = 0;
    virtual void onSceneLoaded() = 0;
    virtual void updateScene(float deltaTime) = 0;
    virtual void processKeyboardInput(float deltaTime) = 0;

    void initialize();

    void loadScene();

    void addDirectionalLight(glm::vec3 position);

    void setAssetsPath(std::string assetsPath);

    void setGLWindow(GLFWwindow* window);

    GLFWwindow* getWindow();

    void setVAOManager(VAOManager* objectManager);

    unsigned int getShaderProgramId();

    LightManager* getLightMananger();

    std::vector<GameObject*> getObjectsToDraw();

    std::vector<GameObject*> mObjectsToDraw;

    VAOManager* objectManager = NULL;
private:
    // Constants
    const std::string defaultScenesPath = "assets/scenes";

    // Variables
    int& shaderProgramId;
    std::string assetsBasePath = defaultScenesPath;

    GLFWwindow* sceneWindow;

    int NUM_OF_LIGHTS = 0;

    LightManager* lightManager = NULL;

    // Functions

};

