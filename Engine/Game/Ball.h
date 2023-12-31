#pragma once

#include "../Graphics/GameObject/GameObject.h"

class Ball: public GameObject  {
public:
	Ball(const std::string& fileName);
	~Ball();

	virtual void update(float deltaTime) override;
	virtual void resolveCollisions() override;

	const float moveSpeed = 10.0f;
	const float rotationSpeed = 1.0f;

	void moveBall(glm::vec3 moveDirection);

	std::vector<GameObject*> debugSpheres;

	std::vector<GameObject*>* mObjectsToDraw;

};

