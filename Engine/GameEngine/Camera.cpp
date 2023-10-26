#include "Camera.h"

Camera::Camera() {}

Camera::~Camera() {}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) : position(position), up(up), yaw(yaw), pitch(pitch) {
    movementSpeed = 0.001f;
    sensitivity = 1.0f;

    front = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::normalize(glm::cross(front, up));
    worldUp = up;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::setGLWindow(GLFWwindow* win) {
    window = win;
}

void Camera::processKeyboardInput(float deltaTime) {
    float velocity = movementSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        position += front * velocity;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        position -= front * velocity;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        position -= right * velocity;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        position += right * velocity;

    // Handle camera movement up and down
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)  // Space for moving up
        position += up * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)  // Left Shift for moving down
        position -= up * velocity;

    // Handle camera rotation
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)  // Q for rotating left
        yaw -= sensitivity;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)  // E for rotating right
        yaw += sensitivity;
    
    // Call updateViewMatrix to keep the camera's orientation up to date
    updateViewMatrix();
}

void Camera::updateCameraVectors() {
    // Calculate the new front vector based on yaw and pitch
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);

    // Recalculate the right and up vectors
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
}

void Camera::updateViewMatrix() { 
    updateCameraVectors();
}