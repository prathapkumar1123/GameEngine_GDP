#include "PhysicsEngine.h"
#include "sPhysicsProperties.h"
#include "iPhysicsMesh.h"

#include <iostream>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> 

bool PhysicsEngine::m_Sphere_Sphere_IntersectionTest(GameObject* sphereA, GameObject* sphereB) {

	sPhysicsProperties::sSphere* pSphereA = (sPhysicsProperties::sSphere*)(sphereA->pShape);
	sPhysicsProperties::sSphere* pSphereB = (sPhysicsProperties::sSphere*)(sphereB->pShape);

	glm::vec3 centerToCenter = sphereB->getDrawPosition() - sphereA->getDrawPosition();

	// Calculate the squared distance between the centers (squared for efficiency)
	float distanceSquared = glm::dot(centerToCenter, centerToCenter);

	// Calculate the sum of the radii
	float sumOfRadii = pSphereA->radius + pSphereB->radius;

	// Check if the squared distance is less than or equal to the squared sum of the radii
	if (distanceSquared <= sumOfRadii * sumOfRadii) {
		// Spheres intersect

		glm::vec3 collisionNormal = glm::normalize(sphereA->drawPosition - sphereB->drawPosition);

		// Calculate relative velocity
		glm::vec3 relativeVelocity = sphereA->velocity - sphereB->velocity;

		sCollisionEvent* collisionEvent = new sCollisionEvent();
		collisionEvent->relativeVelocity = relativeVelocity;
		collisionEvent->collisionNormal = collisionNormal;
		collisionEvent->collidedObjMass = sphereB->inverse_mass;

		sphereA->collisionWith[sphereB->simpleName] = collisionEvent;

		return true;
	}

	sphereA->collisionWith[sphereB->simpleName] = nullptr;
	// Spheres do not intersect
	return false;
}

bool PhysicsEngine::m_Sphere_Plane_IntersectionTest(GameObject* pSphere, GameObject* pPlane) {
	return false;
}

bool PhysicsEngine::m_Sphere_Triangle_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriangle) {
	// TODO: Insert amazing code here 
	return false;
}

bool PhysicsEngine::m_Sphere_AABB_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pAABB) {
	// TODO: Insert amazing code here 
	return false;
}

bool PhysicsEngine::m_Sphere_Capsule_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pCapsule) {
	// TODO: Insert amazing code here 
	return false;
}

bool PhysicsEngine::m_Sphere_TriMeshIndirect_IntersectionTest(GameObject* pSphere_General, GameObject* pTriMesh_General) {

	// Do we have a mesh manager? 
	if (!this->mMeshManager) return false;

	// Get the info about this shape, specifically
	sPhysicsProperties::sMeshOfTriangles_Indirect* pTriangleMesh = (sPhysicsProperties::sMeshOfTriangles_Indirect*)(pTriMesh_General->pShape);
	sPhysicsProperties::sSphere* pSphere = (sPhysicsProperties::sSphere*)(pSphere_General->pShape);

	ModelDrawInfo theMeshDrawInfo;

	// Find the raw mesh information from the VAO manager
	if (!this->mMeshManager->findDrawInfoByModelName(pTriangleMesh->meshName, theMeshDrawInfo)) return false;


	//	glm::vec3 closestPointToTriangle = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	float closestDistanceSoFar = FLT_MAX;
	glm::vec3 closestTriangleVertices[3] = { glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f) };
	glm::vec3 closestContactPoint = glm::vec3(FLT_MAX, FLT_MAX, FLT_MAX);
	unsigned int indexOfClosestTriangle = INT_MAX;

	// We now have the mesh object location and the detailed mesh information 
					// Which triangle is closest to this sphere (right now)
	for (unsigned int index = 0; index != theMeshDrawInfo.NUM_OF_INDICES; index += 3)
	{
		glm::vec3 verts[3];

		// This is TERRIBLE for cache misses
		verts[0].x = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index]].x;
		verts[0].y = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index]].y;
		verts[0].z = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index]].z;

		verts[1].x = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index + 1]].x;
		verts[1].y = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index + 1]].y;
		verts[1].z = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index + 1]].z;

		verts[2].x = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index + 2]].x;
		verts[2].y = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index + 2]].y;
		verts[2].z = theMeshDrawInfo.mVertices[theMeshDrawInfo.mIndices[index + 2]].z;

		glm::mat4 matModel = glm::mat4(1.0f);

		// Translation
		glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
			glm::vec3(pTriMesh_General->drawPosition.x,
				pTriMesh_General->drawPosition.y,
				pTriMesh_General->drawPosition.z));

		glm::mat4 matRotation = glm::mat4(pTriMesh_General->getQuatOrientation());

		// Scaling matrix
		glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
			glm::vec3(pTriMesh_General->drawScale.x,
				pTriMesh_General->drawScale.y,
				pTriMesh_General->drawScale.z));

			// Combine all these transformation
		matModel = matModel * matTranslate;
		matModel = matModel * matRotation;
		matModel = matModel * matScale;

		// vertexWorldPos = matModel * vec4(vPos.xyz, 1.0f);

		glm::vec4 vertsWorld[3];
		vertsWorld[0] = (matModel * glm::vec4(verts[0], 1.0f));
		vertsWorld[1] = (matModel * glm::vec4(verts[1], 1.0f));
		vertsWorld[2] = (matModel * glm::vec4(verts[2], 1.0f));

		// And make sure you multiply the normal by the inverse transpose
		// OR recalculate it right here! 

		// ******************************************************

		glm::vec3 thisTriangleClosestPoint = this->mClosestPtPointTriangle(pSphere_General->drawPosition,
			vertsWorld[0], vertsWorld[1], vertsWorld[2]);

		// Is this the closest so far
		float distanceToThisTriangle = glm::distance(thisTriangleClosestPoint, pSphere_General->drawPosition);

		if (distanceToThisTriangle < closestDistanceSoFar)
		{
			// this one is closer
			closestDistanceSoFar = distanceToThisTriangle;
			// Make note of the triangle index
			indexOfClosestTriangle = index;
			// 
			closestTriangleVertices[0] = vertsWorld[0];
			closestTriangleVertices[1] = vertsWorld[1];
			closestTriangleVertices[2] = vertsWorld[2];

			closestContactPoint = thisTriangleClosestPoint;
		}
	}

	if (closestDistanceSoFar <= pSphere->radius) {

		glm::vec3 sphereDirection = pSphere_General->velocity;

		sphereDirection = glm::normalize(sphereDirection);

		// Calcualte the current normal from the TRANSFORMED vertices
		glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
		glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];

		glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));

		// Calculate the reflection vector from the normal	
		// https://registry.khronos.org/OpenGL-Refpages/gl4/html/reflect.xhtml
		// 1st parameter is the "incident" vector
		// 2nd parameter is the "normal" vector
		glm::vec3 reflectionVec = glm::reflect(sphereDirection, triNormal);

		// Update the  velocity based on this reflection vector
		float sphereSpeed = glm::length(pSphere_General->velocity);
		glm::vec3 newVelocity = reflectionVec * sphereSpeed;

		pSphere_General->velocity = newVelocity;

		return true;
	}

	//if (closestDistanceSoFar <= pSphere->radius) {
	//	glm::vec3 sphereDirection = pSphere_General->velocity;
	//	float sphereSpeed = glm::length(sphereDirection);

	//	// Normalize velocity
	//	sphereDirection = glm::normalize(sphereDirection);

	//	// Calculate the current normal from the TRANSFORMED vertices
	//	glm::vec3 edgeA = closestTriangleVertices[1] - closestTriangleVertices[0];
	//	glm::vec3 edgeB = closestTriangleVertices[2] - closestTriangleVertices[0];

	//	glm::vec3 triNormal = glm::normalize(glm::cross(edgeA, edgeB));

	//	// Calculate the reflection vector from the normal
	//	glm::vec3 reflectionVec = glm::reflect(sphereDirection, triNormal);

	//	// Apply damping to reduce the sphere's velocity
	//	float dampingFactor = 0.3f; // Adjust this value as needed
	//	sphereSpeed *= dampingFactor;

	//	// Update the velocity based on this reflection vector
	//	glm::vec3 newVelocity = reflectionVec * sphereSpeed;

	//	// Apply smoothing to the new velocity (optional)
	//	//float smoothingFactor = 0.01f; // Adjust this value as needed

	//	//pSphere_General->velocity = glm::mix(pSphere_General->velocity, newVelocity, smoothingFactor);

	//	// Create collision event
	//	sCollisionEvent* collision = new sCollisionEvent();
	//	collision->contactPoint = closestContactPoint;
	//	collision->reflectionNormal = reflectionVec;
	//	collision->velocityAtCollision = pSphere_General->velocity;

	//	// float sphereSpeed = glm::length(pSphere_General->velocity);
	//	// glm::vec3 newVelocity = reflectionVec * sphereSpeed;

	//	pSphere_General->velocity = newVelocity;

	//	// Assign collision information to the sphere
	//	// pSphere_General->collisionWith[pTriMesh_General->simpleName] = collision;

	//	return true;
	//}

	// Didn't hit
	return false;
}

bool PhysicsEngine::m_Sphere_TriMeshLocal_IntersectionTest(sPhysicsProperties* pSphere, sPhysicsProperties* pTriMesh)
{
	// TODO: Insert amazing code here 
	return false;
}