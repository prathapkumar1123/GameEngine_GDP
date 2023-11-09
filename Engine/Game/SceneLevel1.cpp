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
	std::cout << "onSceneLoaded" << std::endl;
	BaseScene::addDirectionalLight(glm::vec3(0.0f, 15.0f, 0.0f));

    buildSpheres();

	PhysicsEngine::getInstance().setObjects(mObjectsToDraw);
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

void SceneLevel1::buildSpheres() {
	for (int i = 0; i < 10; i++) {
        GameObject* pObject = new Ball("Sphere_1x1.ply");
		pObject->simpleName = "sphere"; // +std::to_string(i);
        pObject->bDoNotLight = false;
        pObject->bIsWireframe = false;
        glm::vec3 position(genRandomFloat(-5.0f, 5.0f), 15.0f + i, genRandomFloat(-5.0f, 5.0f));
		pObject->drawPosition = position; // glm::vec3(0.0f, 5.0f, 0.0f); // position;

		ModelDrawInfo sphereMeshDrawInfo;
		objectManager->findDrawInfoByModelName(pObject->getFileName(), sphereMeshDrawInfo);

		pObject->size = sphereMeshDrawInfo.size;
		pObject->maxVertex = sphereMeshDrawInfo.maxVertex;
		pObject->minVertex = sphereMeshDrawInfo.minVertex;

		// pObject->velocity = glm::vec3(genRandomFloat(-1.0, 1.0) * 0.01, -0.01, genRandomFloat(-1.0, 1.0) * 0.01);

		if (i == 0) {
			pObject->drawPosition = glm::vec3(0.0f, 10.0f, 0.0f);
			pObject->simpleName = "player";

			playerBall = dynamic_cast<Ball*>(pObject);
		}
		
		pObject->isRigidBody = true;

		float scale = 1.0f;
        pObject->setUniformDrawScale(scale);

		float originalRadius = sphereMeshDrawInfo.size.x / 2;
		float radius = originalRadius * scale;

		pObject->shapeType = SPHERE;

		pObject->pShape = new sPhysicsProperties::sSphere(radius);

        mObjectsToDraw.push_back(pObject);

		//// Debug Sphere
		//glm::vec3 minPoint = (sphereMeshDrawInfo.minVertex * pObject->drawScale) + pObject->drawPosition;
		//glm::vec3 maxPoint = (sphereMeshDrawInfo.maxVertex * pObject->drawScale) + pObject->drawPosition;

		//// Calculate the eight vertices of the bounding box
		//glm::vec3 vertices[8];
		//vertices[0] = glm::vec3(minPoint.x, minPoint.y, minPoint.z);
		//vertices[1] = glm::vec3(maxPoint.x, minPoint.y, minPoint.z);
		//vertices[2] = glm::vec3(maxPoint.x, maxPoint.y, minPoint.z);
		//vertices[3] = glm::vec3(minPoint.x, maxPoint.y, minPoint.z);
		//vertices[4] = glm::vec3(minPoint.x, minPoint.y, maxPoint.z);
		//vertices[5] = glm::vec3(maxPoint.x, minPoint.y, maxPoint.z);
		//vertices[6] = glm::vec3(maxPoint.x, maxPoint.y, maxPoint.z);
		//vertices[7] = glm::vec3(minPoint.x, maxPoint.y, maxPoint.z);

		//for (int i = 0; i < 8; i++) {
		//	GameObject* debugObject = new GameObject("Sphere_1x1.ply");
		//	debugObject->simpleName = "sphere_debug";
		//	debugObject->bDoNotLight = true;
		//	debugObject->bIsWireframe = true;
		//	debugObject->drawPosition = vertices[i];
		//	debugObject->isRigidBody = false;
		//	debugObject->hasCollider = false;
		//	debugObject->shapeType = SPHERE;

		//	debugObject->bUseDebugColours = true;
		//	debugObject->wholeObjectDebugColourRGBA = glm::vec4(genRandomFloat(0.0f, 1.0f), genRandomFloat(0.0f, 1.0f), genRandomFloat(0.0f, 1.0f), 1.0f);

		//	debugObject->setUniformDrawScale(0.1f);

		//	float originalRadius = 1.0f;
		//	float radius = originalRadius * debugObject->scale;

		//	debugObject->pShape = new sPhysicsProperties::sSphere(0.22f);

		//	dynamic_cast<Ball*>(pObject)->debugSpheres.push_back(debugObject);

		//	// mObjectsToDraw.push_back(debugObject);
		//}
	}
}