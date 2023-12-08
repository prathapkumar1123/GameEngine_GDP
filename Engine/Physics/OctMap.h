//#pragma once
//
//#include <glm/glm.hpp>
//#include <vector>
//#include <iostream>
//
//#include "../Graphics/GameObject/GameObject.h"
//#include "../Graphics/VAOManager/VAOManager.h"
//#include "OctaMap.h"
//
//// Octree Node
//struct OctreeNode {
//    BoundingBox3D bounds;
//    std::vector<GameObject*> objects;
//    OctreeNode* children[8];
//
//    OctreeNode(const BoundingBox3D& bounds) : bounds(bounds) {
//        for (int i = 0; i < 8; ++i) {
//            children[i] = nullptr;
//        }
//    }
//
//    ~OctreeNode() {
//        for (int i = 0; i < 8; ++i) {
//            delete children[i];
//        }
//    }
//};
//
//// Octree class
//class Octree {
//public:
//    Octree(const BoundingBox3D& bounds, int maxDepth);
//
//    void Insert(GameObject* object);
//    void Query(const BoundingBox3D& region, std::vector<GameObject*>& result) const;
//    void DetectCollisions(std::vector<GameObject*>& collidingObjects) const;
//
//private:
//    OctreeNode* root;
//    int maxDepth;
//
//    void Insert(OctreeNode* node, GameObject* object, int depth);
//    void Query(OctreeNode* node, const BoundingBox3D& region, std::vector<GameObject*>& result) const;
//    void DetectCollisions(OctreeNode* node, std::vector<GameObject*>& collidingObjects) const;
//};
//
//Octree::Octree(const BoundingBox3D& bounds, int maxDepth) : maxDepth(maxDepth) {
//    root = new OctreeNode(bounds);
//}
//
//void Octree::Insert(GameObject* object) {
//    Insert(root, object, 0);
//}
//
//void Octree::Insert(OctreeNode* node, GameObject* object, int depth) {
//    if (depth == maxDepth) {
//        node->objects.push_back(object);
//        return;
//    }
//
//    int octant = 0;
//    if (object->drawPosition.x > node->bounds.min.x + node->bounds.max.x) octant |= 1;
//    if (object->drawPosition.y > node->bounds.min.y + node->bounds.max.y) octant |= 2;
//    if (object->drawPosition.z > node->bounds.min.z + node->bounds.max.z) octant |= 4;
//
//    if (node->children[octant] == nullptr) {
//        float minX = (octant & 1) ? node->bounds.min.x : (node->bounds.min.x + node->bounds.max.x) / 2.0f;
//        float minY = (octant & 2) ? node->bounds.min.y : (node->bounds.min.y + node->bounds.max.y) / 2.0f;
//        float minZ = (octant & 4) ? node->bounds.min.z : (node->bounds.min.z + node->bounds.max.z) / 2.0f;
//
//        float maxX = (octant & 1) ? (node->bounds.min.x + node->bounds.max.x) / 2.0f : node->bounds.max.x;
//        float maxY = (octant & 2) ? (node->bounds.min.y + node->bounds.max.y) / 2.0f : node->bounds.max.y;
//        float maxZ = (octant & 4) ? (node->bounds.min.z + node->bounds.max.z) / 2.0f : node->bounds.max.z;
//
//        glm::vec3 minPoint(minX, minY, minZ);
//        glm::vec3 maxPoint(maxX, maxY, maxZ);
//        BoundingBox3D childBounds(minPoint, maxPoint);
//        node->children[octant] = new OctreeNode(childBounds);
//    }
//
//    Insert(node->children[octant], object, depth + 1);
//}
//
//void Octree::Query(const BoundingBox3D& region, std::vector<GameObject*>& result) const {
//    Query(root, region, result);
//}
//
//void Octree::Query(OctreeNode* node, const BoundingBox3D& region, std::vector<GameObject*>& result) const {
//    if (node == nullptr) {
//        return;
//    }
//
//    if (node->bounds.min.x > region.max.x || node->bounds.max.x < region.min.x ||
//        node->bounds.min.y > region.max.y || node->bounds.max.y < region.min.y ||
//        node->bounds.min.z > region.max.z || node->bounds.max.z < region.min.z) {
//        return;
//    }
//
//    if (node->objects.size() > 0) {
//        result.insert(result.end(), node->objects.begin(), node->objects.end());
//        return;
//    }
//
//    for (int i = 0; i < 8; ++i) {
//        Query(node->children[i], region, result);
//    }
//}
//
//void Octree::DetectCollisions(std::vector<GameObject*>& collidingObjects) const {
//    DetectCollisions(root, collidingObjects);
//}
//
//void Octree::DetectCollisions(OctreeNode* node, std::vector<GameObject*>& collidingObjects) const {
//    if (node == nullptr) {
//        return;
//    }
//
//    if (node->objects.size() > 1) {
//        // Perform collision detection among the objects in this node
//        for (size_t i = 0; i < node->objects.size(); ++i) {
//            for (size_t j = i + 1; j < node->objects.size(); ++j) {
//                collidingObjects.push_back(node->objects[i]);
//                collidingObjects.push_back(node->objects[j]);
//            }
//        }
//    }
//
//    for (int i = 0; i < 8; ++i) {
//        DetectCollisions(node->children[i], collidingObjects);
//    }
//}