#include "BaseScene.h"

#include "../Core/Globals.h"
#include "../Game/Ball.h"
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

    textureManager = new BasicTextureManager();
    textureManager->SetBasePath(defaultTexturesPath);

    onInit();
}

void BaseScene::loadScene() {
    isSceneLoaded = false;
    using json = nlohmann::json;

    std::ifstream inputFile(assetsBasePath + "/" + sceneFile);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to load the scene file." << std::endl;
        return;
    }

    try {
        mObjectsToDraw.clear();

        json sceneData;
        inputFile >> sceneData;
        inputFile.close();

        json modelData = sceneData["scene"]["models"];
        json cameraData = sceneData["scene"]["camera"];
        json lightData = sceneData["scene"]["lights"];
        json lights = lightData["lights"];

        Camera::getInstance().gCameraEye = glm::vec3(cameraData["cameraEye"][0], cameraData["cameraEye"][1], cameraData["cameraEye"][2]);
        Camera::getInstance().position = glm::vec3(cameraData["position"][0], cameraData["position"][1], cameraData["position"][2]);
        Camera::getInstance().front = glm::vec3(cameraData["front"][0], cameraData["front"][1], cameraData["front"][2]);
        Camera::getInstance().right = glm::vec3(cameraData["right"][0], cameraData["right"][1], cameraData["right"][2]);
        Camera::getInstance().up = glm::vec3(cameraData["up"][0], cameraData["up"][1], cameraData["up"][2]);
        Camera::getInstance().worldUp = glm::vec3(cameraData["worldUp"][0], cameraData["worldUp"][1], cameraData["worldUp"][2]);
        Camera::getInstance().pitch = cameraData.value("pitch", -44.0f);
        Camera::getInstance().yaw = cameraData.value("yaw", -90.0f);
        Camera::getInstance().movementSpeed = cameraData.value("movementSpeed", 1.0f);
        Camera::getInstance().sensitivity = cameraData.value("sensitivity", 1.0f);

        turnOnLights = lightData.value("turnOn", true);
        drawLightsToModels = lightData.value("drawLightsToModel", true);

        for (const auto& model : modelData) {
            std::string name = model.value("name", "");
            std::string file = model.value("file", "");
            std::string meshName = model.value("meshName", "");

            glm::vec3 position = glm::vec3(model["position"][0], model["position"][1], model["position"][2]);
            glm::vec3 scale = glm::vec3(model["scale"][0], model["scale"][1], model["scale"][2]);
            glm::vec3 rotation = glm::vec3(model["rotation"][0], model["rotation"][1], model["rotation"][2]);

            bool setUniformDrawScale = model.value("setUniformDrawScale", true);
            bool isWireframe = model.value("isWireframe", false);
            bool isVisible = model.value("isVisible", true);
            bool useCustomColor = model.value("useCustomColor", true);

            float uniformDrawScale = model.value("uniformDrawScale", 1.0f);
            float inverseMass = model.value("inverseMass", -1.0f);

            Mesh* mesh = nullptr;

            if (model.find("textures") != model.end() && model["textures"].is_array()) {
                std::vector<std::string> textures = model["textures"].get<std::vector<std::string>>();
                mesh = new Mesh(model.value("textureName", ""));

                for (std::size_t index = 0; index < textures.size(); ++index) {
                    loadTextures(textures[index]);
                    mesh->textureName[index] = textures[index];
                }
            }

            GameObject* pObject = new GameObject(file);
            pObject->shapeType = MESH_OF_TRIANGLES_INDIRECT;
            pObject->pShape = new sPhysicsProperties::sMeshOfTriangles_Indirect(file);

            pObject->simpleName = name;
            pObject->bDoNotLight = !drawLightsToModels;
            pObject->drawPosition = position;
            pObject->bIsVisible = isVisible;
            pObject->bIsWireframe = isWireframe;
            pObject->inverse_mass = inverseMass;

            if (useCustomColor) {
                pObject->bUseDebugColors = true;
                glm::vec4 customColor = glm::vec4(model["customColor"][0], model["customColor"][1], model["customColor"][2], model["customColor"][3]);
                pObject->wholeObjectDebugColourRGBA = customColor;
            }

            if (!setUniformDrawScale) {
                if (scale.x != 0 || scale.y != 0 || scale.z != 0) {
                    pObject->drawScale = scale;
                }
            }
            else {
                pObject->setUniformDrawScale(uniformDrawScale);
            }

            pObject->setMesh(mesh);
            ModelDrawInfo drawInfo;
            objectManager->findDrawInfoByModelName(file, drawInfo);

            /*pObject->size = drawInfo.size;
            pObject->maxVertex = drawInfo.maxVertex;
            pObject->minVertex = drawInfo.minVertex;*/

            float degreesToRadians = glm::pi<float>() / 180.0f;

            glm::vec3 radians(0.f);
            radians.x = rotation.x * degreesToRadians;
            radians.y = rotation.y * degreesToRadians;
            radians.z = rotation.z * degreesToRadians;

            pObject->setRotationFromEuler(radians);

            mObjectsToDraw.push_back(pObject);
        }

        int lightNum = 0;
        for (const auto& light : lights) {
            std::string lightType = light.value("type", "");
            glm::vec3 position = glm::vec3(light["position"][0], light["position"][1], light["position"][2]);
            glm::vec3 direction = glm::vec3(light["direction"][0], light["direction"][1], light["direction"][2]);
            float innerAngle = light.value("innerAngle", 0);
            float outerAngle = light.value("outerAngle", 0);
            float constantAttenuation = light.value("constantAttenuation", 0);
            float linearAttenuation = light.value("linearAttenuation", 0);
            float quadraticAttenuation = light.value("quadraticAttenuation", 0);
            glm::vec4 diffuseColor = glm::vec4(light["diffuseColor"][0], light["diffuseColor"][1], light["diffuseColor"][2], light["diffuseColor"][3]);
            glm::vec4 specularColor = glm::vec4(light["specularColor"][0], light["specularColor"][1], light["specularColor"][2], light["diffuseColor"][3]);

            if (lightType == "point") {
                addPointLight(lightNum, position, constantAttenuation, linearAttenuation, quadraticAttenuation, diffuseColor, specularColor);
            }
            else if (lightType == "spot") {
                addSpotLight(lightNum, position, direction, innerAngle, outerAngle, constantAttenuation, linearAttenuation, quadraticAttenuation, diffuseColor, specularColor);
            }
            else if (lightType == "direction") {
                addDirectionalLight(lightNum, position, direction, innerAngle, outerAngle, constantAttenuation, linearAttenuation, quadraticAttenuation, diffuseColor, specularColor);
            }

            lightNum++;
        }

        sceneData = NULL;
    }
    catch (const std::exception& e) {   
        std::cerr << "Error loading scene.." << e.what() << std::endl;
    }

    std::cout << "Scene Loaded" << std::endl;
    this->loadCubeMapTextures();
    this->onSceneLoaded();
}

void BaseScene::onSceneLoaded() {
    isSceneLoaded = true;
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

void BaseScene::loadCubeMapTextures() {
    std::string errors;

    textureManager->SetBasePath(defaultCubeMapTexturesPath);
    textureManager->CreateCubeTextureFromBMPFiles("SunnyDay",
        "TropicalSunnyDayLeft2048.bmp",
        "TropicalSunnyDayRight2048.bmp",
        "TropicalSunnyDayUp2048.bmp",
        "TropicalSunnyDayDown2048.bmp",
        "TropicalSunnyDayFront2048.bmp",
        "TropicalSunnyDayBack2048.bmp",
        true,
        errors);

    textureManager->SetBasePath(defaultTexturesPath);
}

void BaseScene::loadTextures(std::string textureBMPFile) {
    textureManager->SetBasePath(defaultTexturesPath);
    if (!textureManager->Create2DTextureFromBMPFile(textureBMPFile, true)) {
        std::cout << "Loading Texture Failed: " << textureBMPFile << std::endl;
    }
}

void BaseScene::addDirectionalLight(
    int lightNum,
    glm::vec3 position,
    glm::vec3 direction,
    float innerAngle,
    float outerAngle,
    float constantAttenuation,
    float linearAttenuation,
    float quadraticAttenuation,
    glm::vec4 diffuseColor,
    glm::vec4 specularColor
) {
    if (lightManager->NUMBER_OF_LIGHTS_IM_USING > lightNum) {
        lightManager->theLights[lightNum].param2.x = turnOnLights ? 1.0 : 0.0;      // Turn on
        lightManager->theLights[lightNum].param1.x = 2.0f;                          // Set to 2.0 for directional light
        lightManager->theLights[lightNum].direction = glm::vec4(direction, 1.0f);   // Direction for a point light
        lightManager->theLights[lightNum].param1.y = innerAngle;                    // No inner angle needed for a point light
        lightManager->theLights[lightNum].param1.z = outerAngle;                    // No outer angle needed for a point light
        lightManager->theLights[lightNum].atten.x = constantAttenuation;            // Constant attenuation
        lightManager->theLights[lightNum].atten.y = linearAttenuation;              // Linear attenuation (adjust for desired range)
        lightManager->theLights[lightNum].atten.z = quadraticAttenuation;           // Quadratic attenuation (adjust for desired range)
        lightManager->theLights[lightNum].position = glm::vec4(position, 1.0f);

        lightManager->theLights[lightNum].diffuse = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);  // Adjust intensity for diffuse light
        lightManager->theLights[lightNum].specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); // Adjust intensity for specular light

        NUM_OF_LIGHTS++;
    }
}

void BaseScene::addPointLight(
    int lightNum,
    glm::vec3 position,
    float constantAttenuation,
    float linearAttenuation,
    float quadraticAttenuation,
    glm::vec4 diffuseColor,
    glm::vec4 specularColor
) {
    if (lightManager->NUMBER_OF_LIGHTS_IM_USING > lightNum) {
        lightManager->theLights[lightNum].param2.x = turnOnLights ? 1.0 : 0.0; // Turn on
        lightManager->theLights[lightNum].param1.x = 0.0f; // Set to 0.0 for point light
        lightManager->theLights[lightNum].direction = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // No direction for a point light
        lightManager->theLights[lightNum].param1.y = 0.0f; // No inner angle needed for a point light
        lightManager->theLights[lightNum].param1.z = 0.0f; // No outer angle needed for a point light
        lightManager->theLights[lightNum].atten.x = constantAttenuation; // Constant attenuation
        lightManager->theLights[lightNum].atten.y = linearAttenuation; // Linear attenuation (adjust for desired range)
        lightManager->theLights[lightNum].atten.z = quadraticAttenuation; // Quadratic attenuation (adjust for desired range)
        lightManager->theLights[lightNum].position = glm::vec4(position, 1.0f);

        lightManager->theLights[lightNum].diffuse = diffuseColor; // Adjust intensity for diffuse light
        lightManager->theLights[lightNum].specular = specularColor; // Adjust intensity for specular light

        NUM_OF_LIGHTS++;
    }
}

void BaseScene::addSpotLight(
    int lightNum,
    glm::vec3 position,
    glm::vec3 direction,
    float innerAngle,
    float outerAngle,
    float constantAttenuation,
    float linearAttenuation,
    float quadraticAttenuation,
    glm::vec4 diffuseColor,
    glm::vec4 specularColor
) {
    if (lightManager->NUMBER_OF_LIGHTS_IM_USING > lightNum) {
        lightManager->theLights[lightNum].param2.x = turnOnLights ? 1.0 : 0.0; // Turn on
        lightManager->theLights[lightNum].param1.x = 1.0f; // Set to 1.0 for spot light
        lightManager->theLights[lightNum].direction = glm::vec4(direction, 1.0f); // Direction for a point light
        lightManager->theLights[lightNum].param1.y = innerAngle; // No inner angle needed for a point light
        lightManager->theLights[lightNum].param1.z = outerAngle; // No outer angle needed for a point light
        lightManager->theLights[lightNum].atten.x = constantAttenuation; // Constant attenuation
        lightManager->theLights[lightNum].atten.y = linearAttenuation; // Linear attenuation (adjust for desired range)
        lightManager->theLights[lightNum].atten.z = quadraticAttenuation; // Quadratic attenuation (adjust for desired range)
        lightManager->theLights[lightNum].position = glm::vec4(position, 1.0f);

        lightManager->theLights[lightNum].diffuse = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f); // Adjust intensity for diffuse light
        lightManager->theLights[lightNum].specular = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f); // Adjust intensity for specular light

        NUM_OF_LIGHTS++;
    }
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