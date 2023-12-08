#pragma once

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

struct sCollisionEvent {
	float collidedObjRadius = 0.0f;

	float collidedObjMass;
	glm::vec3 collisionNormal;
	glm::vec3 relativeVelocity;

	glm::vec3 colObjMinVertex;
	glm::vec3 colObjMaxVertex;

	glm::vec3 contactPoint;
	glm::vec3 velocityAtCollision;
	glm::vec3 reflectionNormal;
};

class iPhysicsMesh
{
public:
	virtual ~iPhysicsMesh() {};

	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 rotation;
	float inverse_mass = 0.0f;

	virtual glm::vec3 getDrawPosition(void) = 0;
	virtual glm::vec3 getDrawOrientation(void) = 0;

	virtual glm::vec3 getRotation(void) = 0;

	virtual void setDrawPosition(const glm::vec3& newPosition) = 0;
	virtual void setDrawOrientation(const glm::vec3& newOrientation) = 0;
	virtual void setDrawOrientation(const glm::quat& newOrientation) = 0;

	virtual glm::quat getQuatOrientation(void) = 0;
	
	// Rotation
	virtual void setRotation(glm::vec3 rotation) = 0;
	virtual void setRotationFromEuler(glm::vec3 newEulerAngleXYZ) = 0;
	virtual void adjustRotationAngleFromEuler(glm::vec3 EulerAngleXYZ_Adjust) = 0;

};
