#include "GameObject.h"

#include <iostream>

unsigned int GameObject::mNextUniqueID = GameObject::FIRST_UNIQUE_ID;

GameObject::GameObject(const std::string& fileName) {
	this->fileName = fileName;

	this->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->drawScale = glm::vec3(1.0f);
	this->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));

	this->bIsWireframe = false;
	this->bDoNotLight = false;
	this->bIsVisible = true;
	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	this->inverse_mass = -1.0f;

	this->mUniqueID = mNextUniqueID;
	mNextUniqueID++;
}

GameObject::~GameObject() {}

std::string GameObject::getFileName() {
	return fileName;
}

unsigned int GameObject::getUniqueID(void) {
	return this->mUniqueID;
}

glm::vec3 GameObject::getDrawPosition(void) {
	return this->drawPosition;
}

glm::vec3 GameObject::getRotation() {
	return rotation;
}

void GameObject::setRotation(glm::vec3 rotation) {
	this->rotation = rotation;
}

void GameObject::setRotationFromEuler(glm::vec3 newEulerAngleXYZ) {
	this->quatOrientation = glm::quat(newEulerAngleXYZ);
	this->rotation = newEulerAngleXYZ;
}

void GameObject::adjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust) {
	glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
	this->quatOrientation *= qChange;
}

glm::quat GameObject::getQuatOrientation(void) {
	return this->quatOrientation;
}

void GameObject::setUniformDrawScale(float scale) {
	this->drawScale.x = this->drawScale.y = this->drawScale.z = scale;
	return;
}

void GameObject::setDrawPosition(const glm::vec3& newPosition) {
	this->drawPosition = newPosition;
	return;
}

glm::vec3 GameObject::getDrawOrientation(void) {
	return glm::eulerAngles(this->getQuatOrientation());
}

void GameObject::setDrawOrientation(const glm::vec3& newOrientation) {
	this->setRotationFromEuler(newOrientation);
	return;
}

void GameObject::setDrawOrientation(const glm::quat& newOrientation) {
	this->quatOrientation = newOrientation;
	return;
}

void GameObject::onCollided(GameObject* collisionWith, bool isCollided) {

}

void GameObject::update(float deltatime) {

}

void GameObject::resolveCollisions() {

}