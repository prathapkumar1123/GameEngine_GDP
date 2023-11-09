#include "Ball.h"

#include <iostream>

Ball::Ball(const std::string& fileName): GameObject(fileName) {
	this->velocity = glm::vec3(0.0f, 0.005f, 0.0f);
	this->inverse_mass = 1.0f;
	this->acceleration = glm::vec3(0.0f, -0.001f, 0.0f);
	this->hasCollider = true;
}

Ball::~Ball() {

}

void Ball::onCollided(GameObject* collisionWith, bool isCollided) {
//
//	float restitution = 0.8f;
//	if (collisionWith->simpleName == "floor" || collisionWith->simpleName == "stairs") {
//		if (isCollided) {
//			this->velocity.y *= -restitution;
//			this->velocity.x *= 0.9f; // Apply horizontal friction.
//
//			this->acceleration = glm::vec3(0.0f, -0.001f, 0.0f); // Reset acceleration for gravity.
//
//			if (this->velocity.y <= 0) {
//				this->acceleration = glm::vec3(0.0f);
//				this->velocity = glm::vec3(0.0f);
//			}
//		}
//		else {
//			// No collision with "floor," apply gravity.
//			this->acceleration = glm::vec3(0.0f, -0.001f, 0.0f);
//		}
//	}
}


void Ball::update(float deltaTime) {
	// NewVelocity = LastVel + (Accel * DeltaTime)
	// NewPosition = LastPos + (Vel * DeltaTime)

	this->velocity += this->acceleration * deltaTime;
	this->drawPosition += this->velocity * deltaTime;
}

void Ball::moveBall(glm::vec3 moveDirection) {
	if (collisionWith["floor"] != nullptr) {
		this->drawPosition += moveDirection * moveSpeed;
		this->setRotationFromEuler(this->getRotation() + 0.1f);
	}
}

void Ball::resolveCollisions() {
    float restitution = 0.8f;

    // Check if there is no collision with "floor" or "stairs."
    if (collisionWith["floor"] == nullptr && collisionWith["stairs"] == nullptr) {
        // Apply gravity.
        this->acceleration = glm::vec3(0.0f, -0.001f, 0.0f);
    }
    else {
        if (this->velocity.y < 0) {
            // Handle collision with "floor" or "stairs."
            this->velocity.y *= -restitution;
            this->velocity.x *= 0.7f; // Apply horizontal friction.

            // Reset acceleration to simulate gravity.
            this->acceleration = glm::vec3(0.0f, -0.001f, 0.0f);
        }

        //// Ensure the ball stays on top of the "floor" or "stairs."
        //float ballRadius = /* specify the radius of the ball */;
        //if (this->drawPosition.y < ballRadius) {
        //    this->drawPosition.y = ballRadius;
        //}
    }
}