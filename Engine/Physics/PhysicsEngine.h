#pragma once

#include "../Physics/sPhysicsProperties.h"
#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/VAOManager/VAOManager.h"

#include <vector>

class PhysicsEngine
{
public:

	static PhysicsEngine& getInstance() {
		static PhysicsEngine instance;
		return instance;
	}

	// Delete the copy constructor and assignment operator to prevent duplication
	PhysicsEngine(const PhysicsEngine&) = delete;
	void operator=(const PhysicsEngine&) = delete;

	~PhysicsEngine();

	void updatePhysics(double deltaTime);

	void setVAOManager(VAOManager* pMeshManager);

	void setObjects(std::vector<GameObject*> objects);

	GameObject* findShapeByUniqueID(unsigned int uniqueIDtoFind);
	GameObject* findShapeByFriendlyName(std::string friendlyNameToFind);

	// There's a ray cast code in chapter 5 of Ericson's book.
	bool rayCast(glm::vec3 startXYZ, glm::vec3 endXYZ, std::vector<sPhysicsProperties*>& vecObjectsHit);

	std::vector<GameObject*> mCollisionObjects;

private:
	PhysicsEngine();

	bool m_Sphere_Sphere_IntersectionTest(GameObject* sphereA, GameObject* sphereB);
	bool m_Sphere_Plane_IntersectionTest(GameObject* pSphere, GameObject* pPlane);
	bool m_Sphere_Triangle_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriangle);
	bool m_Sphere_AABB_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pAABB);
	bool m_Sphere_Capsule_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pCapsule);
	bool m_Sphere_TriMeshIndirect_IntersectionTest(GameObject* pSphere, GameObject* pTriMesh);
	bool m_Sphere_TriMeshLocal_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh);

	glm::vec3 closestTriangle(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& p);

	// The basic tests
	bool mTestSphereTriangle(float sphereRadius, glm::vec3 vert0, glm::vec3 vert1, glm::vec3 vert2, glm::vec3 sphereCentre);
	glm::vec3 mClosestPtPointTriangle(glm::vec3 p, glm::vec3 a, glm::vec3 b, glm::vec3 c);

	VAOManager* mMeshManager = NULL;
};

