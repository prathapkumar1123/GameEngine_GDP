#include "Mesh.h"

#include <iostream>

unsigned int Mesh::m_nextUniqueID = Mesh::FIRST_UNIQUE_ID;

Mesh::Mesh(const std::string& meshName) {
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
	this->m_UniqueID = m_nextUniqueID;
	m_nextUniqueID++;
}

Mesh::~Mesh() {}

std::string Mesh::getMeshName() {
	return meshName;
}

unsigned int Mesh::getUniqueID(void) {
	return this->m_UniqueID;
}

void Mesh::setUniformDrawScale(float scale) {
	this->drawScale.x = this->drawScale.y = this->drawScale.z = scale;
	return;
}

glm::vec3 Mesh::getDrawPosition(void) {
	return this->drawPosition;
}

void Mesh::setDrawPosition(const glm::vec3& newPosition) {
	this->drawPosition = newPosition;
	return;
}

glm::vec3 Mesh::getDrawOrientation(void) {
	return glm::eulerAngles(this->get_qOrientation());
}

void Mesh::setDrawOrientation(const glm::vec3& newOrientation) {
	this->setRotationFromEuler(newOrientation);
	return;
}

void Mesh::setDrawOrientation(const glm::quat& newOrientation) {
	this->m_qOrientation = newOrientation;
	return;
}