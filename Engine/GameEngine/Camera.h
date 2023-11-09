#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Core/OpenGlCommons.h"

class Camera {
public:
    static Camera& getInstance() {
        static Camera instance;
        return instance;
    }

    // Delete the copy constructor and assignment operator to prevent duplication
    Camera(const Camera&) = delete;
    void operator=(const Camera&) = delete;

    ~Camera();

    glm::mat4 getViewMatrix();

    void setGLWindow(GLFWwindow* window);

    void processKeyboardInput(float deltaTime);

    void updateViewMatrix();

    glm::vec3 gCameraEye;

    glm::vec3 position;

    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

private:
    Camera();

    void updateCameraVectors();

    float movementSpeed = 0.01f;
    float sensitivity = 1.0f;

    GLFWwindow* window;
};

