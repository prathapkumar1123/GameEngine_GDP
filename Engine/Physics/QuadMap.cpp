#include "QuadMap.h"



QuadMap::QuadMap(glm::vec3 center, int size, VAOManager* gameObjectManager) {
    root = new QuadNode();
    root->center = center;

    root->children = std::vector<QuadNode*>(CAPACITY, nullptr); 
}

QuadMap::~QuadMap() {}

void QuadMap::Insert(GameObject* object) {
    InsertInNode(root, object);
}

void QuadMap::InsertInNode(QuadNode* node, GameObject* object) {
    if (!IsObjectInNode(node, object)) {
        return;
    }

    if (node->children[0] == nullptr) {

        node->objects.push_back(object);

        if (node->objects.size() > CAPACITY) {
            SubDivideNode(node);
        }

        return;
    }
    else {
        for (int i = 0; i < CAPACITY; i++) {
            InsertInNode(node->children[i], object);
        }
    }
}

void QuadMap::SubDivideNode(QuadNode* node) {
    for (int i = 0; i < CAPACITY; i++) {
        QuadNode* child = new QuadNode();
        child->children = std::vector<QuadNode*>(CAPACITY, nullptr);

        glm::vec3 offset;
        offset.x = (i & 1) ? node->size / 4 : -node->size / 4;
        offset.y = (i & 2) ? node->size / 4 : -node->size / 4;
        offset.z = (i & 4) ? node->size / 4 : -node->size / 4;

        child->center = node->center + offset;

        node->children[i] = child;

        // Initialize the children's children as empty vectors
        node->children[i]->children = std::vector<QuadNode*>(CAPACITY, nullptr);
    }
}

bool QuadMap::IsObjectInNode(const QuadNode* node, const GameObject* object) {
    ModelDrawInfo info;
    this->gameObjectManager->findDrawInfoByModelName(object->simpleName, info);

    //// Calculate the minimum and maximum coordinates of the object's bounding box
    glm::vec3 objectMin = object->drawPosition - 0.5f * info.size;
    glm::vec3 objectMax = object->drawPosition + 0.5f * info.size;

    // Calculate the minimum and maximum coordinates of the node
    glm::vec3 nodeMin = node->center - 0.5f * node->size;
    glm::vec3 nodeMax = node->center + 0.5f * node->size;

    // Check if all dimensions of the object's bounding box are within the node's dimensions
    if (objectMin.x >= nodeMin.x && objectMax.x <= nodeMax.x &&
        objectMin.y >= nodeMin.y && objectMax.y <= nodeMax.y &&
        objectMin.z >= nodeMin.z && objectMax.z <= nodeMax.z) {
        return true;
    }

    return false;
}

void QuadMap::Remove(GameObject* object) {}

void QuadMap::Update(GameObject* object) {}

void QuadMap::Clear() {}

void QuadMap::RenderDebug() {}

void QuadMap::QueryCollision(GameObject* object, std::vector<GameObject*>& collisionObjects) {}