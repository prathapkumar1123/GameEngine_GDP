#pragma once

#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/VAOManager/VAOManager.h"
#include "../Graphics/LightsManager/LightManager.h"
#include "../GameEngine/Camera.h"

#include <string>
#include <iostream>

class Scene{

public:

    LightManager* lightManager = NULL;

    std::vector<GameObject*> getObjectsToDraw();

    unsigned int getShaderProgramId();

    void setGLWindow(GLFWwindow* window);

    //void setCamera(Camera* camera);

    void loadScene();

    void toggleSceneLights(bool turnOn);

    void updateScene();

    void processKeyboardInput(float deltaTime);

    void setAssetsPath(std::string assetsPath);

    // Builder
    class Builder {
    public:
        Builder(int& shaderProgramId) : shaderProgramId(shaderProgramId) {

        }

        Builder& withAssetsBasePath(std::string assetsBasePath) {
            this->assetsBasePath = assetsBasePath;
            return *this;
        }

        Builder& setSceneFile(std::string sceneFile) {
            this->sceneFile = sceneFile;
            return *this;
        }

        Scene* build() {
            return new Scene(shaderProgramId, sceneFile);
        }

    private:
        const std::string defaultScenesPath = "assets/scenes";
        int& shaderProgramId;
        std::string sceneFile;
        std::string assetsBasePath = defaultScenesPath;
    };

private:
    Scene(int& shaderProgramId, std::string sceneFile);

    ~Scene();

    const std::string defaultScenesPath = "assets/scenes";

    // Camera& camera;

    GLFWwindow* sceneWindow;
    int& shaderProgramId;

    std::string sceneFile;

    std::string assetsBasePath = defaultScenesPath;
    std::vector<GameObject*> mObjectsToDraw;

    float SPEED = 0.1f;
};