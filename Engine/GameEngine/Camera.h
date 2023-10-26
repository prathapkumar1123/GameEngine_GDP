#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Core/OpenGlCommons.h"

class Camera {

public:
    Camera();
    ~Camera();

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch);

    glm::mat4 getViewMatrix();

    void setGLWindow(GLFWwindow* window);

    void processKeyboardInput(float deltaTime);

    void updateViewMatrix();

    void updateCameraVectors();

    float yaw;
    float pitch;
    float movementSpeed;
    float sensitivity;

private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    GLFWwindow* window;
};

