#include "Camera.h"

#include <iostream>

Camera::Camera(): gCameraEye(0.0f, 0.0f, 10.0f), yaw(0.0f), pitch(0.0f), position(0.0f, 0.0f, 10.0f),
front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f),
right(1.0f, 0.0f, 0.0f), worldUp(0.0f, 1.0f, 0.0f), window(nullptr) {
    movementSpeed = 0.01f;
    sensitivity = 1.5f;
    window = NULL;

    front = glm::vec3(0.0f, 0.0f, -1.0f);
    right = glm::normalize(glm::cross(front, up));
    worldUp = up;
}

Camera::~Camera() {}

glm::mat4 Camera::getViewMatrix() {
    std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
    return glm::lookAt(position, position + front, up);
}

void Camera::setGLWindow(GLFWwindow* win) {
    std::cout << "Setting Window..." << std::endl;
    window = win;
}

void Camera::processKeyboardInput(float deltaTime) {
    if (window != NULL) {
        float velocity = movementSpeed * deltaTime;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            position += front * velocity;
        }
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

// Define a callback function for mouse input
//void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//    // Calculate the change in mouse position
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos;  // Reversed since y-coordinates range from bottom to top
//
//    // Update lastX and lastY for the next frame
//    lastX = xpos;
//    lastY = ypos;
//
//    // Adjust the camera's yaw and pitch based on mouse movement
//    camera.yaw += xoffset * camera.sensitivity;
//    camera.pitch += yoffset * camera.sensitivity;
//
//    // Limit pitch to avoid flipping the camera
//    if (camera.pitch > 89.0f) camera.pitch = 89.0f;
//    if (camera.pitch < -89.0f) camera.pitch = -89.0f;
//
//    // Update the camera's front vector
//    camera.updateCameraVectors();
//}