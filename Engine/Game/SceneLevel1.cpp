#include "SceneLevel1.h"

#include "../GameEngine/BaseScene.h"
#include "../Physics/PhysicsEngine.h"

#include <iostream>
#include <random>

std::random_device rd;
std::mt19937 gen(rd());

float genRandomFloat(float min = -100.0f, float max = 0.0f) {
    std::uniform_real_distribution<float> dist(min, max);
    float randomValue = dist(gen);
    return randomValue;
}

SceneLevel1::SceneLevel1(int shaderProgramId, std::string sceneFile) : BaseScene(shaderProgramId, sceneFile) {
	std::cout << "SceneLevel1" << std::endl;
	initialize();
}

void SceneLevel1::onInit() {
	std::cout << "OnInit" << std::endl;
}

void SceneLevel1::onSceneLoaded() {
	isSceneLoaded = true;
	std::cout << "onSceneLoaded" << std::endl;

    //buildSpheres();
	PhysicsEngine::getInstance().setObjects(&mObjectsToDraw);
}

void SceneLevel1::updateScene(float deltaTime) {
	// std::cout << "updateScene" << std::endl;

	if (getLightMananger() != nullptr)
		getLightMananger()->UpdateUniformValues(getShaderProgramId());

}

void SceneLevel1::processKeyboardInput(float deltaTime) {
	Camera::getInstance().processKeyboardInput(deltaTime);

	if (playerBall != nullptr) {
		glm::vec3 moveDirection(0.0f);

		if (glfwGetKey(getWindow(), GLFW_KEY_W) == GLFW_PRESS)
			moveDirection = glm::vec3(0.0f, 0.0f, -0.1f);
		if (glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS)
			moveDirection = glm::vec3(0.0f, 0.0f, 0.1f);
		if (glfwGetKey(getWindow(), GLFW_KEY_A) == GLFW_PRESS)
			moveDirection = glm::vec3(-0.1f, 0.0f, 0.0f);
		if (glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS)
			moveDirection = glm::vec3(0.1f, 0.0f, 0.0f);

		if (glfwGetKey(getWindow(), GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS)
			moveDirection = glm::vec3(0.1f, 0.0f, -0.1f);
		if (glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS && glfwGetKey(getWindow(), GLFW_KEY_D) == GLFW_PRESS)
			moveDirection = glm::vec3(0.1f, 0.0f, 0.1f);
		if (glfwGetKey(getWindow(), GLFW_KEY_A) == GLFW_PRESS && glfwGetKey(getWindow(), GLFW_KEY_S) == GLFW_PRESS)
			moveDirection = glm::vec3(-0.1f, 0.0f, 0.1f);
		if (glfwGetKey(getWindow(), GLFW_KEY_W) == GLFW_PRESS && glfwGetKey(getWindow(), GLFW_KEY_A) == GLFW_PRESS)
			moveDirection = glm::vec3(-0.1f, 0.0f, -0.1f);

		if (moveDirection != glm::vec3(0.0f)) {
			// Calculate the rotation angle based on movement direction.
			float rotationAngle = glm::atan(moveDirection.x, moveDirection.z);

			// Rotate the ball.
			playerBall->setRotationFromEuler(glm::vec3(rotationAngle, rotationAngle, rotationAngle));

			// Move the ball.
			playerBall->moveBall(moveDirection);
		}
	}
}

//void SceneLevel1::loadTextures(std::string textureBMPFile) {}
//
//void SceneLevel1::loadCubeMapTextures() {
//	BaseScene::loadCubeMapTextures();
//}

void SceneLevel1::buildSpheres() {
	for (int i = 0; i < 1; i++) {
        GameObject* pObject = new Ball("Sphere_1x1.ply");
		pObject->simpleName = "sphere"; // +std::to_string(i);
        pObject->bDoNotLight = false;
        pObject->bIsWireframe = false;
        glm::vec3 position(genRandomFloat(-1.0f, 1.0f), 30.0f + i, genRandomFloat(-1.0f, 1.0f));
		pObject->drawPosition = position; // glm::vec3(0.0f, 5.0f, 0.0f); // position;

		ModelDrawInfo sphereMeshDrawInfo;
		objectManager->findDrawInfoByModelName(pObject->getFileName(), sphereMeshDrawInfo);

		/*pObject->size = sphereMeshDrawInfo.size;
		pObject->maxVertex = sphereMeshDrawInfo.maxVertex;
		pObject->minVertex = sphereMeshDrawInfo.minVertex;*/

		if (i == 0) {
			pObject->drawPosition = glm::vec3(40.0f, 30.0f, 65.0f);
			pObject->simpleName = "player";

			playerBall = dynamic_cast<Ball*>(pObject);
			playerBall->mObjectsToDraw = &mObjectsToDraw;
		}

		float scale = 1.0f;
        pObject->setUniformDrawScale(scale);

		//float originalRadius = sphereMeshDrawInfo.size.x / 2;
		//float radius = originalRadius * scale;

		pObject->shapeType = SPHERE;
	}
}