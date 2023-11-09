#include "PhysicsEngine.h"
#include "OctaMap.h"

#include <iostream>

PhysicsEngine::PhysicsEngine() {}

PhysicsEngine::~PhysicsEngine() {}

void PhysicsEngine::setVAOManager(VAOManager* mMeshManager) {
	this->mMeshManager = mMeshManager;
}

void PhysicsEngine::setObjects(std::vector<GameObject*> objects) {
	this->mCollisionObjects = objects;
}


GameObject* PhysicsEngine::findShapeByUniqueID(unsigned int uniqueIDtoFind) {
	for (GameObject* pObject : this->mCollisionObjects){
		if (pObject->getUniqueID() == uniqueIDtoFind) {
			return pObject;
		}
	}

	return NULL;
}

GameObject* PhysicsEngine::findShapeByFriendlyName(std::string friendlyNameToFind) {
	for (GameObject* pObject : this->mCollisionObjects) {
		if (pObject->simpleName == friendlyNameToFind) {
			return pObject;
		}
	}

	return NULL;
}

// There's a ray cast code in chapter 5 of Ericson's book.
// What does this return? 
// It could return all the things that the ray hit.
// You could add some methods that only ray cast through specific objects (like only meshes)
// You could also do something where it returns the "closest" object (maybe form the startXYZ)
bool PhysicsEngine::rayCast(glm::vec3 startXYZ, glm::vec3 endXYZ, std::vector<sPhysicsProperties*>& vecObjectsHit) {
	return false;
}

void PhysicsEngine::updatePhysics(double deltaTime) {

	std::map<std::string, GameObject*> collisionObjects;

	for (GameObject* colObj : mCollisionObjects) {

		if (colObj->inverse_mass >= 0.0f) {
			colObj->update(deltaTime);
		}

		glm::vec3 minPoint = colObj->getBoundingBoxMin();
		glm::vec3 maxPoint = colObj->getBoundingBoxMax();

		BoundingBox3D aBounds = BoundingBox3D(minPoint, maxPoint);

		for (GameObject* sceneObj : mCollisionObjects) {

			if (colObj->simpleName == sceneObj->simpleName) {
				continue;
			}

			if (!colObj->isRigidBody || !sceneObj->isRigidBody) {
				continue;
			}

			BoundingBox3D bBounds = BoundingBox3D(sceneObj->getBoundingBoxMin(), sceneObj->getBoundingBoxMax());

			if (aBounds.intersects(bBounds)) {

				if (colObj->shapeType == SPHERE &&
					sceneObj->shapeType == MESH_OF_TRIANGLES_INDIRECT) {
					if (!this->m_Sphere_TriMeshIndirect_IntersectionTest(colObj, sceneObj)) {
						colObj->collisionWith[sceneObj->simpleName] = nullptr;
					}
				}

				if (colObj->shapeType == SPHERE &&
					sceneObj->shapeType == SPHERE) {
					if (!this->m_Sphere_Sphere_IntersectionTest(sceneObj, colObj)) {
						colObj->collisionWith[sceneObj->simpleName] = nullptr;
					}
				}
			}
			else {
				colObj->collisionWith[sceneObj->simpleName] = nullptr;
			}
		
		}

		colObj->resolveCollisions();
	}

}