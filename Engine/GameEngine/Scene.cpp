#include "Scene.h"

#include "../Core/Globals.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

Scene::Scene(int& shaderProgamId, std::string sceneFile): shaderProgramId(shaderProgamId) {
	this->shaderProgramId = shaderProgamId;
    this->sceneFile = sceneFile;

    // camera = Camera::getInstance();

    lightManager = new LightManager();

    toggleSceneLights(true);
}

Scene::~Scene() {}

std::vector<GameObject*> Scene::getObjectsToDraw() {
    return mObjectsToDraw;
}

unsigned int Scene::getShaderProgramId() {
    return shaderProgramId;
}

void printJSON(GameObject* o) {
    json jsonObj;

    jsonObj["name"] = o->simpleName;
    jsonObj["file"] = o->getMeshName();

    jsonObj["position"][0] = o->drawPosition.x;
    jsonObj["position"][1] = o->drawPosition.y;
    jsonObj["position"][2] = o->drawPosition.z;

    jsonObj["scale"][0] = o->drawScale.x;
    jsonObj["scale"][1] = o->drawScale.y;
    jsonObj["scale"][2] = o->drawScale.z;

    jsonObj["rotation"][0] = o->getRotation().x;
    jsonObj["rotation"][1] = o->getRotation().y;
    jsonObj["rotation"][2] = o->getRotation().z;

    jsonObj["setUniformDrawScale"] = true;
    jsonObj["uniformDrawScale"] = 1.0f;

    std::string jsonString = jsonObj.dump();

    std::cout << jsonString << "," << std::endl;

}

void Scene::loadScene() {
    std::ifstream inputFile(assetsBasePath + "/" + sceneFile);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to load the scene file." << std::endl;
        return;
    }

    json sceneData;
    inputFile >> sceneData;
    inputFile.close();

    json modelData = sceneData["scene"];

    for (const auto& model : modelData["models"]) {
        std::string name = model.value("name", "");
        std::string file = model.value("file", "");


        glm::vec3 position = glm::vec3(model["position"][0], model["position"][1], model["position"][2]);
        glm::vec3 scale = glm::vec3(model["scale"][0], model["scale"][1], model["scale"][2]);
        glm::vec3 rotation = glm::vec3(model["rotation"][0], model["rotation"][1], model["rotation"][2]);

        bool setUniformDrawScale = model.value("setUniformDrawScale", true);
        float uniformDrawScale = model.value("uniformDrawScale", 1.0f);

        GameObject* pObject = new GameObject(file);
        pObject->simpleName = name;
        pObject->bDoNotLight = true;
        pObject->drawPosition = position;

        if (scale.x != 0 || scale.y != 0 || scale.z != 0)
            pObject->drawScale = scale;

        if (setUniformDrawScale)
            pObject->setUniformDrawScale(uniformDrawScale);

        float degreesToRadians = glm::pi<float>() / 180.0f;

        glm::vec3 radians;
        radians.x = rotation.x * degreesToRadians;
        radians.y = rotation.y * degreesToRadians;
        radians.z = rotation.z * degreesToRadians;

        pObject->setRotationFromEuler(radians);

        mObjectsToDraw.push_back(pObject);
    }
}

void Scene::toggleSceneLights(bool turnOn) {
    float turn = 1.0f;

    if (turnOn) {
        turn = 1.0f;
    } else {
        turn = 0.0f;
    }

    lightManager->theLights[0].param2.x = turn;   // Turn on
    lightManager->theLights[0].param1.x = 0.0f;   // Set to 0 for point light
    lightManager->theLights[0].direction = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f); // No direction needed for point light
    lightManager->theLights[0].param1.y = 0.0f;  // No inner angle needed
    lightManager->theLights[0].param1.z = 0.0f;  // No outer angle needed
    lightManager->theLights[0].position.x = 15.0f; // Set the position where the point light originates
    lightManager->theLights[0].position.y = 20.0f;
    lightManager->theLights[0].position.z = 30.0f;
    lightManager->theLights[0].atten.x = 10.0f;        // Constant attenuation
    lightManager->theLights[0].atten.y = 0.001f;       // Linear attenuation
    lightManager->theLights[0].atten.z = 0.000001f;    // Quadratic attenuation
}

void Scene::updateScene() {
    Camera::getInstance().updateViewMatrix();

    if (lightManager != nullptr)
        lightManager->UpdateUniformValues(shaderProgramId);
}

void Scene::setGLWindow(GLFWwindow* window) {
    this->sceneWindow = window;
    Camera::getInstance().setGLWindow(window);
}

void Scene::processKeyboardInput(float deltaTime) {
    Camera::getInstance().processKeyboardInput(deltaTime);
}

//void Scene::setCamera(Camera& camera) {
//    this->camera = camera;
//}

void Scene::setAssetsPath(std::string assetsPath) {
    this->assetsBasePath = assetsPath;
}