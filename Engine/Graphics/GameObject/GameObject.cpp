#include "GameObject.h"

#include <iostream>

unsigned int GameObject::mNextUniqueID = GameObject::FIRST_UNIQUE_ID;

GameObject::GameObject(const std::string& meshName) {
	this->meshName = meshName;

	this->drawPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	this->setRotationFromEuler(glm::vec3(0.0f, 0.0f, 0.0f));
	this->drawScale = glm::vec3(1.0f);

	this->bIsWireframe = false;
	this->bDoNotLight = false;
	this->bIsVisible = true;
	this->bUseDebugColours = false;
	this->wholeObjectDebugColourRGBA = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Set uniqueID
	this->mUniqueID = mNextUniqueID;
	mNextUniqueID++;
}

GameObject::~GameObject() {}

std::string GameObject::getMeshName() {
	return meshName;
}

unsigned int GameObject::getUniqueID(void) {
	return this->mUniqueID;
}

void GameObject::setUniformDrawScale(float scale) {
	this->drawScale.x = this->drawScale.y = this->drawScale.z = scale;
	return;
}

glm::vec3 GameObject::getDrawPosition(void) {
	return this->drawPosition;
}

void GameObject::setDrawPosition(const glm::vec3& newPosition) {
	this->drawPosition = newPosition;
	return;
}

glm::vec3 GameObject::getDrawOrientation(void) {
	return glm::eulerAngles(this->get_qOrientation());
}

void GameObject::setDrawOrientation(const glm::vec3& newOrientation) {
	this->setRotationFromEuler(newOrientation);
	return;
}

void GameObject::setDrawOrientation(const glm::quat& newOrientation) {
	this->m_qOrientation = newOrientation;
	return;
}