#include "Core/OpenGlCommons.h"
#include "Graphics/VAOManager/VAOManager.h"
#include "Graphics/ShaderManager/ShaderManager.h"
#include "Core/GLFW_Callbacks.h"
#include "GameEngine/Camera.h"
#include "GameEngine/GameEngine.h"
#include "GameEngine/BaseScene.h"
#include "Game/SceneLevel1.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>

//float lastX;
//float lastY;

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
//    camera->yaw += xoffset * camera->sensitivity;
//    camera->pitch += yoffset * camera->sensitivity;
//
//    // Limit pitch to avoid flipping the camera
//    if (camera->pitch > 89.0f) camera->pitch = 89.0f;
//    if (camera->pitch < -89.0f) camera->pitch = -89.0f;
//
//    // Update the camera's front vector
//    camera->updateCameraVectors();
//}

int main() {
	std::cout << "Please wait, Starting the scene..." << std::endl;
    GameEngine engine = GameEngine();
    engine.setFPSRate(FPS_30);

    int shaderProgramId = engine.getShaderProgramId();
    /*Scene* scene = Scene::Builder(shaderProgramId)
        .setSceneFile("scene_1.json")
        .build();*/

   
    SceneLevel1 scene(engine.getShaderProgramId(), "scene_1.json");
    BaseScene* baseScenePtr = &scene;

    engine.setCurrentScene(&scene);
    engine.init();
    engine.runGameLoop();
    engine.stop();

    return 0;
}