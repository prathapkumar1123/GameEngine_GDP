#include "BaseScene.h"

#include "../Core/Globals.h"
#include "../GameEngine/Camera.h"
#include "../Physics/sPhysicsProperties.h"

#include <iostream>
#include <fstream>

#include <nlohmann/json.hpp>

BaseScene::BaseScene(int& shaderProgramId, std::string sceneFile) : shaderProgramId(shaderProgramId) {
    this->sceneFile = sceneFile;
}

BaseScene::~BaseScene() {}

void BaseScene::initialize() {
    lightManager = new LightManager();
    lightManager->SetUniformLocations(shaderProgramId);

    onInit();
}

void BaseScene::loadScene() {

    using json = nlohmann::json;

    std::ifstream inputFile(assetsBasePath + "/" + sceneFile);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to load the scene file." << std::endl;
        return;
    }

    json sceneData;
    inputFile >> sceneData;
    inputFile.close();

    json modelData = sceneData["scene"]["models"];

    for (const auto& model : modelData) {
        std::string name = model.value("name", "");
        std::string file = model.value("file", "");
        std::string meshName = model.value("meshName", "");

        bool isMesh = model.value("isMesh", false);
        bool isVisible = model.value("isVisible", false);

        glm::vec3 position = glm::vec3(model["position"][0], model["position"][1], model["position"][2]);
        glm::vec3 scale = glm::vec3(model["scale"][0], model["scale"][1], model["scale"][2]);
        glm::vec3 rotation = glm::vec3(model["rotation"][0], model["rotation"][1], model["rotation"][2]);

        bool setUniformDrawScale = model.value("setUniformDrawScale", true);
        float uniformDrawScale = model.value("uniformDrawScale", 1.0f);

        GameObject* pObject = new GameObject(file);
        pObject->simpleName = name;
        pObject->bDoNotLight = true;
        pObject->drawPosition = position;
        pObject->mesh = meshName;
        pObject->bIsVisible = true;
        pObject->bIsWireframe = false;
        pObject->isRigidBody = true;

        if (scale.x != 0 || scale.y != 0 || scale.z != 0) {
            pObject->drawScale = scale;
        }

        if (setUniformDrawScale)
        pObject->setUniformDrawScale(uniformDrawScale);

        ModelDrawInfo drawInfo;
        objectManager->findDrawInfoByModelName(file, drawInfo);

        pObject->size = drawInfo.size;
        pObject->maxVertex = drawInfo.maxVertex;
        pObject->minVertex = drawInfo.minVertex;

        // pObject->setUniformDrawScale(uniformDrawScale);

        float degreesToRadians = glm::pi<float>() / 180.0f;

        glm::vec3 radians;
        radians.x = rotation.x * degreesToRadians;
        radians.y = rotation.y * degreesToRadians;
        radians.z = rotation.z * degreesToRadians;

        pObject->setRotationFromEuler(radians);

        pObject->shapeType = MESH_OF_TRIANGLES_INDIRECT;

        pObject->pShape = new sPhysicsProperties::sMeshOfTriangles_Indirect(file);

        mObjectsToDraw.push_back(pObject);
    }

    std::cout << "Scene Loaded" << std::endl;
    onSceneLoaded();
}

void BaseScene::onSceneLoaded() {

}

std::vector<GameObject*> BaseScene::getObjectsToDraw() {
    return mObjectsToDraw;
}

LightManager* BaseScene::getLightMananger() {
    return lightManager;
}

GLFWwindow* BaseScene::getWindow() {
    return sceneWindow;
}

void BaseScene::addDirectionalLight(glm::vec3 position) {
    lightManager->theLights[NUM_OF_LIGHTS].param2.x = 1.0; // Turn on
    lightManager->theLights[NUM_OF_LIGHTS].param1.x = 2.0f; // Set to 1.0 for directional light
    lightManager->theLights[NUM_OF_LIGHTS].direction = glm::vec4(0.0f, -90.0f, 0.0f, 0.0f); // No specific direction for a point light
    lightManager->theLights[NUM_OF_LIGHTS].param1.y = 0.0f; // No inner angle needed for a directional light
    lightManager->theLights[NUM_OF_LIGHTS].param1.z = 0.0f; // No outer angle needed for a directional light
    lightManager->theLights[NUM_OF_LIGHTS].atten.x = 1.0f; // Constant attenuation
    lightManager->theLights[NUM_OF_LIGHTS].atten.y = 0.05f; // Linear attenuation (0 for directional light)
    lightManager->theLights[NUM_OF_LIGHTS].atten.z = 0.01f; // Quadratic attenuation (0 for directional light)
    lightManager->theLights[NUM_OF_LIGHTS].position = glm::vec4(position, 1.0f);

    // Direction with respect of the light.
    lightManager->theLights[NUM_OF_LIGHTS].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    lightManager->theLights[NUM_OF_LIGHTS].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    lightManager->theLights[NUM_OF_LIGHTS].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    //// Light #1 is a directional light 
    //::g_pTheLights->theLights[1].param2.x = 1.0f;   // Turn on
    //::g_pTheLights->theLights[1].param1.x = 2.0f;   // 0 = point light

    //// Direction with respect of the light.
    //::g_pTheLights->theLights[1].direction = glm::vec4(0.0f, -1.0f, 0.0f, 1.0f);
    //::g_pTheLights->theLights[1].diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //::g_pTheLights->theLights[1].specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    NUM_OF_LIGHTS++;
}

unsigned int BaseScene::getShaderProgramId() {
    return shaderProgramId;
}

void BaseScene::setAssetsPath(std::string assetsPath) {
    this->assetsBasePath = assetsPath;
}

void BaseScene::setGLWindow(GLFWwindow* window) {
    this->sceneWindow = window;
    Camera::getInstance().setGLWindow(window);
}

void BaseScene::setVAOManager(VAOManager* objectManager) {
    this->objectManager = objectManager;
}