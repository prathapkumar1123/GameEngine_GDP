#pragma once

#include <glm/glm.hpp>
#include <vector>
#include "../Graphics/GameObject/GameObject.h"
#include "../Graphics/VAOManager/VAOManager.h"

struct QuadNode {
    glm::vec3 center;
    int size;
    std::vector<GameObject*> objects; // List of game objects in this node
    std::vector<QuadNode*> children; // 3D quadtree, so there are 8 children nodes
};

class QuadMap {
public:
    QuadMap(glm::vec3 center, int size, VAOManager* gameObjectManager);
    ~QuadMap();

    void Insert(GameObject* object);
    void Remove(GameObject* object);
    void Update(GameObject* object);
    
    void Clear();
    void QueryCollision(GameObject* object, std::vector<GameObject*>& collisionObjects);
    void RenderDebug(); // Optional, for visualization

private:
    QuadNode* root;

    // Define private helper functions to implement the above public functions

    const int CAPACITY = 8;

    VAOManager* gameObjectManager = NULL;

    void SubDivideNode(QuadNode* node);

    void InsertInNode(QuadNode* node, GameObject* object);

    bool IsObjectInNode(const QuadNode* node, const GameObject* object);
};