#include "Globals/OpenGlCommons.h"
#include "Camera.h"

#include <string>
#include <iostream>
#include <vector>
#include "Mesh.h"


extern int g_selectedMesh;
extern std::vector<Mesh*> g_vec_pMeshesToDraw;

extern glm::vec3 gCameraEye;


void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }


    const float CAMERA_MOVEMENT_SPEED = 1.0f;
    const float OBJECT_MOVEMENT_SPEED = 0.01f;
    const float LIGHT_MOVEMENT_SPEED = 1.0f;

    // Is the shift key down
    if (mods == GLFW_MOD_SHIFT)
    {
        // Shift key ONLY is down
    }

    // & will "mask" off the mod leaving the shift
    if ((mods & GLFW_MOD_CONTROL) == GLFW_MOD_CONTROL)
    {
        // Shift key down (ignores other keys)

    }

    // Nothing down
    if (mods == 0)
    {
        // Shift key is NOT down

        if (key == GLFW_KEY_A && action)
        {
            ::gCameraEye.x -= CAMERA_MOVEMENT_SPEED;
        }
        if (key == GLFW_KEY_D && action)
        {
            ::gCameraEye.x += CAMERA_MOVEMENT_SPEED;
        }

        if (key == GLFW_KEY_W && action)
        {
            ::gCameraEye.z += CAMERA_MOVEMENT_SPEED;
        }
        if (key == GLFW_KEY_S && action)
        {
            ::gCameraEye.z -= CAMERA_MOVEMENT_SPEED;
        }


        if (key == GLFW_KEY_Q && action)
        {
            ::gCameraEye.y -= CAMERA_MOVEMENT_SPEED;
        }
        if (key == GLFW_KEY_E && action)
        {
            ::gCameraEye.y += CAMERA_MOVEMENT_SPEED;
        }

    }// if ( ( mods & GLFW_MOD_SHIFT ) == GLFW_MOD_SHIFT )



    return;
}