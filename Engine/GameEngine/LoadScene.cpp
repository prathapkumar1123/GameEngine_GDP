#include "../Graphics/GameObject/Mesh.h"
#include "../Graphics/VAOManager/VAOManager.h"

#include <filesystem>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>

#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

extern std::vector<Mesh*> mMeshesToDraw;

extern VAOManager* mMeshManager;

glm::vec3 degreesToRadians(glm::vec3 angle) {
    float degreesToRadians = glm::pi<float>() / 180.0f;

    // Convert each component of the input vector from degrees to radians
    glm::vec3 radians;
    radians.x = angle.x * degreesToRadians;
    radians.y = angle.y * degreesToRadians;
    radians.z = angle.z * degreesToRadians;

    return radians;
}

void loadModel(unsigned int& shaderProgramId, const char* modelName) {
    ModelDrawInfo modeInfo;
    ::mMeshManager->loadModelIntoVAO(modelName, modeInfo, shaderProgramId);
    std::cout << "Loaded: " << modeInfo.NUM_OF_VERTICES << " vertices" << std::endl;
}

void loadModelsIntoVAO(unsigned int& shaderProgramId) {
    loadModel(shaderProgramId, "banner.ply");
    loadModel(shaderProgramId, "floor.ply");
    loadModel(shaderProgramId, "wall.ply");
    loadModel(shaderProgramId, "barrel.ply");
    loadModel(shaderProgramId, "block.ply");
    loadModel(shaderProgramId, "chest.ply");
    loadModel(shaderProgramId, "column.ply");
    loadModel(shaderProgramId, "floor_detail.ply");
    loadModel(shaderProgramId, "floor_large.ply");
    loadModel(shaderProgramId, "gate.ply");
    loadModel(shaderProgramId, "stairs.ply");
    loadModel(shaderProgramId, "stones.ply");
    loadModel(shaderProgramId, "trap.ply");
    loadModel(shaderProgramId, "wall_half.ply");
    loadModel(shaderProgramId, "wall_narrow.ply");
    loadModel(shaderProgramId, "wall_opening.ply");
    loadModel(shaderProgramId, "wood_structure.ply");
}

void loadMesh(
    std::string meshName,
    std::string simpleName,
    glm::vec3 drawPos,
    glm::vec3 drawScale = glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f),
    bool setUniformDrawScale = true,
    float uniformDrawScale = 1.0f
) {
    Mesh* pMesh = new Mesh(meshName);
    pMesh->simpleName = simpleName;
    pMesh->bDoNotLight = true;
    pMesh->drawPosition = drawPos;

    if (drawScale.x != 0 || drawScale.y != 0 || drawScale.z != 0)
        pMesh->drawScale = drawScale;

    if (setUniformDrawScale)
        pMesh->setUniformDrawScale(uniformDrawScale);

    pMesh->setRotationFromEuler(degreesToRadians(rotation));

    mMeshesToDraw.push_back(pMesh);
}

void buildScene(std::string sceneFile) {

    std::ifstream inputFile("assets/scenes/" + sceneFile);
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

        Mesh* pMesh = new Mesh(file);
        pMesh->simpleName = name;
        pMesh->bDoNotLight = true;
        pMesh->drawPosition = position;

        if (scale.x != 0 || scale.y != 0 || scale.z != 0)
            pMesh->drawScale = scale;

        if (setUniformDrawScale)
            pMesh->setUniformDrawScale(uniformDrawScale);

        pMesh->setRotationFromEuler(degreesToRadians(rotation));

        mMeshesToDraw.push_back(pMesh);
    }

    //glm::vec3 vec0 = glm::vec3(0.0f, 0.0f, 0.0f);
    //glm::vec3 vec1 = glm::vec3(1.0f, 1.0f, 1.0f);

    //// Floor
    //loadMesh("floor.ply", "floor", vec0, glm::vec3(7.0f, 1.0f, 5.0f), vec0, false);
    //loadMesh("floor_large.ply", "floor_large", glm::vec3(0.0f, -0.01f, 0.0f), glm::vec3(200.0f, 1.0f, 200.0f), glm::vec3(0.0f, 0.0f, 180.0f), false);
    //loadMesh("floor_detail.ply", "floor_detail_1", glm::vec3(0.5f, 0.0f, 1.5f), vec1, vec0, false);
    //loadMesh("floor_detail.ply", "floor_detail_2", glm::vec3(-1.5f, 0.0f, -0.5f), vec1, vec0, false);

    //// Front Wall
    //loadMesh("wall.ply", "front_wall_corner_1", glm::vec3(3.0f, 0.0f, -2.0f));
    //loadMesh("wall.ply", "front_wall_corner_2", glm::vec3(3.0f, 0.0f, 2.0f));
    //loadMesh("wall_opening.ply", "front_wall_opening", glm::vec3(3.0f, 0.0f, 1.0f), vec1, glm::vec3(0.0f, 90.0f, 0.0f));
    //loadMesh("wall_half.ply", "front_wall_block_4", glm::vec3(3.0f, 0.0f, -1.0f), vec1, glm::vec3(0.0f, 90.0f, 0.0f));
    //loadMesh("wall_half.ply", "front_wall_block_3", glm::vec3(3.0f, 0.0f, 0.0f), vec1, glm::vec3(0.0f, 90.0f, 0.0f));

    //// Side Wall
    //loadMesh("wall.ply", "side_wall_corner_3", glm::vec3(-3.0f, 0.0f, -2.0f));
    //for (int i = 0; i < 4; i++) {
    //    loadMesh("wall_narrow.ply", "side_wall_block_" + std::to_string(i), glm::vec3((i-2) * 1.0f, 0.0f, -2.0f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));
    //}

    //loadMesh("wall_half.ply", "side_wall_block_5", glm::vec3(2.0f, 0.0f, -2.0f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));

    //// Blocks
    //loadMesh("block.ply", "block_1", glm::vec3(2.0f, 0.0f, -1.0f));

    //// Stairs
    //loadMesh("stairs.ply", "stairs", glm::vec3(1.0f, 0.0f, -1.0f), vec1, glm::vec3(0.0f, 270.0f, 0.0f));

    //// Gate
    //loadMesh("gate.ply", "gate", glm::vec3(2.7f, 0.75f, 1.0f), vec1, glm::vec3(0.0f, 270.0f, 0.0f));

    //loadMesh("wood_structure.ply", "wood_structure", glm::vec3(2.0f, 0.0f, 2.0f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));
    //loadMesh("wood_structure.ply", "wood_structure", glm::vec3(2.0f, 0.0f, 0.0f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));

    //loadMesh("column.ply", "column_1", glm::vec3(-1.7f, 0.0f, 2.2f));
    //loadMesh("column.ply", "column_2", glm::vec3(-3.0f, 0.0f, 0.3f));

    //// Banners
    //loadMesh("banner.ply", "banner_2", glm::vec3(0.0f, -0.1f, -1.85f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));
    //loadMesh("banner.ply", "banner_2", glm::vec3(-1.0f, -0.1f, -1.85f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));
    //loadMesh("banner.ply", "banner_3", glm::vec3(-2.0f, -0.1f, -1.85f), vec1, glm::vec3(0.0f, 180.0f, 0.0f));

    //// Items
    //loadMesh("barrel.ply", "barrel_1", glm::vec3(-3.0f, 0.0f, 1.2f));
    //loadMesh("barrel.ply", "barrel_2", glm::vec3(-2.4f, 0.0f, 2.2f));
    //loadMesh("stones.ply", "stones", glm::vec3(-3.0f, 0.0f, 2.0f));
    //loadMesh("chest.ply", "chest", glm::vec3(-2.7f, 0.0f, 1.6f), vec1, glm::vec3(0.0f, 160.0f, 0.0f));

}