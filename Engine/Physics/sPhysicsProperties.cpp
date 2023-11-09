#include "sPhysicsProperties.h"

sPhysicsProperties::sPhysicsProperties() {/*
	this->velocity = glm::vec3(0.0f);
	this->acceleration = glm::vec3(0.0f);
	this->rotation = glm::vec3(0.0f);
	this->inverse_mass = 1.0f;
	this->shapeType = UNKNOWN_OR_UNDEFINED;

	this->mUniqueId = mNextUniqueId;
	mNextUniqueId++;*/
}

//std::string sPhysicsProperties::getShapeTypeAsString(void)
//{
//	switch (this->shapeType)
//	{
//	case SPHERE:
//		return "SPHERE";
//		break;
//	case PLANE:
//		return "PLANE";
//		break;
//	case TRIANGLE:
//		return "TRIANGLE";
//		break;
//	case AABB:
//		return "AABB";
//		break;
//	case CAPSULE:
//		return "CAPSULE";
//		break;
//	case MESH_OF_TRIANGLES_INDIRECT:
//		return "MESH_OF_TRIANGLES_INDIRECT";
//		break;
//	case MESH_OF_TRIANGLES_LOCAL_VERTICES:
//		return "MESH_OF_TRIANGLES_LOCAL_VERTICES";
//		break;
//	case UNKNOWN_OR_UNDEFINED:
//		return "UNKNOWN_OR_UNDEFINED";
//		break;
//	}
//
//	// ERROR: UNKNOWN_OR_UNDEFINED
//	return "ERROR: Invalid shape type";
//}


//unsigned int sPhysicsProperties::getUniqueId(void) {
//	return this->mUniqueId;
//}
//
////static 
//unsigned int sPhysicsProperties::mNextUniqueId = sPhysicsProperties::FIRST_UNIQUE_ID;

//void sPhysicsProperties::setShape(const sSphere* pSphereProps) {
//	this->pShape = (void*) pSphereProps;
//	return;
//}
//
//void sPhysicsProperties::setShape(const sAABB* pAABB) {
//	this->pShape = (void*)pAABB;
//	return;
//}
//
//void sPhysicsProperties::setShape(const sPlane* pPlaneProps) {
//	this->pShape = (void*) pPlaneProps;
//	return;
//}
//
//void sPhysicsProperties::setShape(const sTriangle* pTriangleProps) {
//	this->pShape = (void*) pTriangleProps;
//	return;
//}
//
//void sPhysicsProperties::setShape(const sCapsule* pCapsuleProps) {
//	this->pShape = (void*) pCapsuleProps;
//	return;
//}

//void sPhysicsProperties::setShape(const sMeshOfTriangles_Indirect* pTriangleMeshProps) {
//	this->pShape = (void*) pTriangleMeshProps;
//	return;
//}
//
//void sPhysicsProperties::setShape(const sMeshOfTriangles_LocalVertices* pTriangleMeshProps) {
//	this->pShape = (void*)pTriangleMeshProps;
//	return;
//}