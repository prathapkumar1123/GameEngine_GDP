#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>

class GameObject {

public:
	GameObject(const std::string& meshName);
	~GameObject();

	std::string simpleName;

	glm::vec3 drawPosition;
	glm::vec3 drawScale;

	std::string getMeshName();

	void setRotation(glm::vec3 rotation) {
		this->rotation = rotation;
	}

	void setRotationFromEuler(glm::vec3 newEulerAngleXYZ) {
		this->m_qOrientation = glm::quat(newEulerAngleXYZ);
	}

	void adjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust) {
		glm::quat qChange = glm::quat(EulerAngleXYZ_Adjust);
		this->m_qOrientation *= qChange;
	}

	glm::quat get_qOrientation(void) {
		return this->m_qOrientation;
	}

	glm::vec3 getRotation() {
		return rotation;
	}

	void setUniformDrawScale(float scale);

	glm::vec3 getDrawPosition(void);
	glm::vec3 getDrawOrientation(void);

	void setDrawPosition(const glm::vec3& newPosition);
	void setDrawOrientation(const glm::vec3& newOrientation);
	void setDrawOrientation(const glm::quat& newOrientation);

	// ENDOF: iPhysicsMeshTransformAccess interface

	bool bIsVisible;
	bool bUseDebugColours;
	bool bIsWireframe;
	bool bDoNotLight;

	glm::vec4 wholeObjectDebugColourRGBA;

	std::vector<GameObject*> vec_pChildMeshes;

	unsigned int getUniqueID(void);

private:
	std::string meshName;

	glm::vec3 rotation;
	glm::quat m_qOrientation;

	unsigned int mUniqueID;
	static unsigned int mNextUniqueID;

	static const unsigned int FIRST_UNIQUE_ID = 1000;
};