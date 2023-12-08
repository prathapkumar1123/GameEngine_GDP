#include "Ball.h"
#include "../GameEngine/Camera.h"

#include <iostream>

Ball::Ball(const std::string& fileName): GameObject(fileName) {
	this->velocity = glm::vec3(0.0f, 0.01f, 0.0f);
	this->inverse_mass = 1.0f;
	this->acceleration = glm::vec3(0.0f, -0.001f, 0.0f);
}

Ball::~Ball() {

}

void Ball::update(float deltaTime) {
	// NewVelocity = LastVel + (Accel * DeltaTime)
	// NewPosition = LastPos + (Vel * DeltaTime)

    this->velocity += this->acceleration * (float) deltaTime;
    this->drawPosition += this->velocity * (float) deltaTime;
}

void Ball::moveBall(glm::vec3 moveDirection) {
    this->drawPosition += moveDirection * moveSpeed;
    this->setRotationFromEuler(this->getRotation() + 0.1f);
}

void Ball::resolveCollisions() {
    for (const auto& collisionPair : collisionWith) {

    }
}