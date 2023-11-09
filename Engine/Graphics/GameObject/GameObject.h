#pragma once

#include "../../Physics/iPhysicsMesh.h"
#include "../../Physics/sPhysicsProperties.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <map>

class GameObject : public iPhysicsMesh {
public:
	GameObject(const std::string& fileName);
	~GameObject();

	std::string simpleName;
	std::string mesh;
	glm::vec3 drawPosition;
	glm::vec3 drawScale;

	bool isMesh;
	bool bIsVisible;
	bool bUseDebugColours;
	bool bIsWireframe;
	bool bDoNotLight;
	bool isRigidBody;
	bool hasCollider;

	glm::vec3 size;
	glm::vec3 minVertex;
	glm::vec3 maxVertex;

	glm::vec4 wholeObjectDebugColourRGBA;
	std::vector<GameObject*> vecChildMeshes;

	eShape shapeType = UNKNOWN_OR_UNDEFINED;

	void* pShape;

	unsigned int getUniqueID(void);
	std::string getFileName();

	void setUniformDrawScale(float scale);

	// iPhysics Mesh
	virtual glm::vec3 getDrawPosition() override;
	virtual glm::vec3 getDrawOrientation() override;
	virtual glm::vec3 getRotation() override;

	virtual void setDrawPosition(const glm::vec3& newPosition) override;
	virtual void setDrawOrientation(const glm::vec3& newOrientation) override;
	virtual void setDrawOrientation(const glm::quat& newOrientation) override;

	// Rotation
	virtual void setRotation(glm::vec3 rotation) override;
	virtual void setRotationFromEuler(glm::vec3 newEulerAngleXYZ) override;
	virtual void adjustRoationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust) override;

	virtual glm::quat getQuatOrientation(void) override;

	virtual void onCollided(GameObject* collisionWith, bool isCollided);
	virtual void update(float deltaTime);

	virtual void resolveCollisions();

	std::map<std::string, sCollisionEvent*> collisionWith;

	// Function to get the bounding box
	glm::vec3 getBoundingBoxMin() const {
		glm::vec3 minPoint = (minVertex * this->drawScale) + this->drawPosition;
		return glm::vec3(minPoint);
	}

	glm::vec3 getBoundingBoxMax() const {
		glm::vec3 maxPoint = (maxVertex * this->drawScale) + this->drawPosition;
		return maxPoint;
	}

private:
	std::string fileName;
	glm::vec3 rotation;
	glm::quat quatOrientation;

	unsigned int mUniqueID;
	static unsigned int mNextUniqueID;
	static const unsigned int FIRST_UNIQUE_ID = 1000;

	glm::mat4 getTransformMatrix() const {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), drawPosition);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), drawScale);
		return translation * scale;
	}
};