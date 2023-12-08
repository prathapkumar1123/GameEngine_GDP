#pragma once

#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/LightsManager/LightManager.h"
#include "../Graphics/TextureManager/cBasicTextureManager.h"
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

    bool isSceneLoaded = false;

    virtual void onInit() = 0;
    virtual void onSceneLoaded() = 0;
    virtual void updateScene(float deltaTime) = 0;
    virtual void processKeyboardInput(float deltaTime) = 0;
    void loadTextures(std::string textureBMPFile);
    void loadCubeMapTextures();

    void initialize();

    void loadScene();

    void addPointLight(
        int lightNum,
        glm::vec3 position,
        float constantAttenuation,
        float linearAttenuation,
        float quadraticAttenuation,
        glm::vec4 diffuseColor,
        glm::vec4 specularColor);

    void addDirectionalLight(
        int lightNum,
        glm::vec3 position,
        glm::vec3 direction,
        float innerAngle,
        float outerAngle,
        float constantAttenuation,
        float linearAttenuation,
        float quadraticAttenuation,
        glm::vec4 diffuseColor,
        glm::vec4 specularColor);

    void addSpotLight(
        int lightNum,
        glm::vec3 position,
        glm::vec3 direction,
        float innerAngle,
        float outerAngle,
        float constantAttenuation,
        float linearAttenuation,
        float quadraticAttenuation,
        glm::vec4 diffuseColor,
        glm::vec4 specularColor);

    void setAssetsPath(std::string assetsPath);

    void setGLWindow(GLFWwindow* window);

    GLFWwindow* getWindow();

    void setVAOManager(VAOManager* objectManager);

    unsigned int getShaderProgramId();

    LightManager* getLightMananger();

    std::vector<GameObject*> getObjectsToDraw();

    std::vector<GameObject*> mObjectsToDraw;

    VAOManager* objectManager = NULL;

    BasicTextureManager* textureManager = NULL;

    const std::string defaultScenesPath = "assets/scenes";
    const std::string defaultTexturesPath = "assets/textures";
    const std::string defaultCubeMapTexturesPath = "assets/textures/cubemaps";

private:
    // Constants

    // Variables
    int& shaderProgramId;
    std::string assetsBasePath = defaultScenesPath;

    GLFWwindow* sceneWindow = nullptr;

    LightManager* lightManager = NULL;

    bool turnOnLights = false;
    bool drawLightsToModels = false;

    int NUM_OF_LIGHTS = 0;

};

