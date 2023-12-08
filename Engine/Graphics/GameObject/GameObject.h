#pragma once

#include "../../Physics/iPhysicsMesh.h"
#include "../../Physics/sPhysicsProperties.h"
#include "Mesh.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <vector>
#include <map>

class GameObject : public iPhysicsMesh {
public:
	GameObject(const std::string& fileName);
	~GameObject();

	std::string simpleName;
	glm::vec3 drawPosition;
	glm::vec3 drawScale;

	bool bIsVisible;
	bool bUseDebugColors;
	bool bIsWireframe;
	bool bDoNotLight;
	bool isStatic;

	bool isOutOfBounds = false;
	bool isDestroyed = false;

	glm::vec3 size;
	glm::vec3 minVertex;
	glm::vec3 maxVertex;

	glm::vec4 wholeObjectDebugColourRGBA;
	std::vector<GameObject*> vecChildMeshes;

	eShape shapeType = UNKNOWN_OR_UNDEFINED;

	void* pShape;

	unsigned int getUniqueID(void);
	std::string getFileName();
	void setFileName(std::string fileName);
	void setUniformDrawScale(float scale);

	Mesh* getMesh();
	void setMesh(Mesh* mesh);

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
	virtual void adjustRotationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust) override;

	virtual glm::quat getQuatOrientation(void) override;

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

	Mesh* mesh;

	glm::mat4 getTransformMatrix() const {
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), drawPosition);
		glm::mat4 scale = glm::scale(glm::mat4(1.0f), drawScale);
		return translation * scale;
	}
};