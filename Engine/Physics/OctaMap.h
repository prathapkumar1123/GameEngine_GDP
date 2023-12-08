#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/VAOManager/VAOManager.h"

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/VAOManager/VAOManager.h"

class OctreeNode {
public:
    int depth;
    glm::vec3 min, max;
    std::vector<GameObject*> objects;
    std::vector<OctreeNode*> children;

    OctreeNode(const glm::vec3& _min, const glm::vec3& _max) : min(_min), max(_max) {
        children.resize(8, nullptr);
    }
};

class Octree {
public:
    Octree(const glm::vec3& minBounds, const glm::vec3& maxBounds, int maxDepth) : maxDepth(maxDepth) {
        root = new OctreeNode(minBounds, maxBounds);
    }

    ~Octree() {
        clear();
        delete root;
    }

    void clear() {
        clearRecursive(root);
    }

    void insert(GameObject* gameObject) {
        insertRecursive(root, gameObject, 0);
    }

    std::map<std::string, GameObject*> checkCollisions(const BoundingBox3D& queryBox) {
        std::map<std::string, GameObject*> collisions;
        checkCollisionsRecursive(root, collisions);
        // findCollisionsInLeafNodes(root, collisions);
        // detectCollisions();
        return collisions;
    }

private:
    OctreeNode* root;
    int maxDepth;

    void clearRecursive(OctreeNode* node) {
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                clearRecursive(node->children[i]);
                delete node->children[i];
                node->children[i] = nullptr;
            }
        }
    }

    void insertRecursive(OctreeNode* node, GameObject* gameObject, int depth) {
        node->depth = depth;
        BoundingBox3D nodeBounds(node->min, node->max);
        BoundingBox3D gameObjectBB = BoundingBox3D(gameObject->getBoundingBoxMin(), gameObject->getBoundingBoxMax());

        // Check if the object intersects with the current node's bounds
        if (!nodeBounds.intersects(gameObjectBB)) {
            return;
        }

        // If the depth is equal to the maximum depth, insert the object into the current node
        if (depth == maxDepth) {
            node->objects.push_back(gameObject);
            return;
        }

        // Continue to insert the object into child nodes
        for (int i = 0; i < 8; i++) {
            if (node->children[i] == nullptr) {
                createChildNode(node, i);
            }
            insertRecursive(node->children[i], gameObject, depth + 1);
        }

        node->objects.push_back(gameObject);
    }

    void createChildNode(OctreeNode* parent, int index) {
        glm::vec3 mid = 0.5f * (parent->min + parent->max);
        int i, j, k;

        i = (index & 1) ? 1 : 0;
        j = (index & 2) ? 1 : 0;
        k = (index & 4) ? 1 : 0;

        glm::vec3 min, max;
        min.x = i ? mid.x : parent->min.x;
        min.y = j ? mid.y : parent->min.y;
        min.z = k ? mid.z : parent->min.z;
        max.x = i ? parent->max.x : mid.x;
        max.y = j ? parent->max.y : mid.y;
        max.z = k ? parent->max.z : mid.z;

        parent->children[index] = new OctreeNode(min, max);
    }

    void detectCollisions(OctreeNode* node, const GameObject& object, std::vector<GameObject*>& collidedObjects) {
        // Check for collisions with objects in this node
        BoundingBox3D objBB = BoundingBox3D(object.getBoundingBoxMin(), object.getBoundingBoxMax());

        for (GameObject* otherObject : node->objects) {
            BoundingBox3D otherObjBB = BoundingBox3D(otherObject->getBoundingBoxMin(), otherObject->getBoundingBoxMax());

            if (objBB.intersects(otherObjBB)) {
                collidedObjects.push_back(otherObject);
            }
        }

        // Recursively check for collisions in the child nodes
        for (OctreeNode* child : node->children) {
            BoundingBox3D childBounds = BoundingBox3D(child->min, child->max);
            if (child && childBounds.intersects(objBB)) {
                detectCollisions(child, object, collidedObjects);
            }
        }
    }

    void checkCollisionsRecursive(OctreeNode* node, std::map<std::string, GameObject*>& collisions) {
        BoundingBox3D nodeBounds(node->min, node->max);

        // Check for collisions with objects stored in this node.
        for (GameObject* obj : node->objects) {

            BoundingBox3D objBB = BoundingBox3D(obj->getBoundingBoxMin(), obj->getBoundingBoxMax());

            if (nodeBounds.intersects(objBB) && node->objects.size() > 1 && node->depth == maxDepth) {
                collisions[obj->simpleName] = obj;
                /*std::cout << obj->simpleName << " - Collision Detected at: " << node->min.x << ", " << node->min.y << ", " << node->min.z << " : "
                    << node->max.x << ", " << node->max.y << ", " << node->max.z << std::endl;*/
            }
        }

        // Recursively check for collisions in child nodes.
        for (int i = 0; i < 8; i++) {
            if (node->children[i] != nullptr) {
                checkCollisionsRecursive(node->children[i], collisions);
            }
        }
    }
};