#include "GLRenderer.h"

#include "../Core/Globals.h"
#include "../Physics/PhysicsEngine.h"
#include "../GameEngine/BaseScene.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLRenderer::GLRenderer() {
    mGameObjectManager = new VAOManager();
    mGameObjectManager->setBasePath(modelsBasePath);

    PhysicsEngine::getInstance().setVAOManager(mGameObjectManager);

    shaderManager = new ShaderManager();

    initOpenGL();
}

GLRenderer::~GLRenderer() {

}

GLFWwindow* GLRenderer::getWindow() {
	return window;
}

void GLRenderer::setVertexShader(std::string name) {
	vertexShader.setShaderFileName(name);
}

void GLRenderer::setfragmentShader(std::string name) {
	fragmentShader.setShaderFileName(name);
}

void GLRenderer::setCurrentScene(BaseScene* currentScene) {
    std::cout << "Current Scene Setting..." << std::endl;
    this->currentScene = currentScene;

    currentScene->setGLWindow(window);
}

Camera& GLRenderer::getCamera() {
    return Camera::getInstance();
}

int GLRenderer::getShaderProgramId() {
    return shaderProgramId;
}

void GLRenderer::setWindowWidth(int width) {
    this->windowWidth = width;
}

void GLRenderer::setWindoHeight(int height) {
    this->windowHeight = height;
}

void GLRenderer::setWindowTitle(std::string title) {
    this->windowTitle = title;
}

void GLRenderer::setShadersBasePath(std::string basePath) {
    this->shaderBasePath = basePath;
}

void GLRenderer::setFPSRate(FPS_RATE fpsRate) {
    this->CURRENNT_FPS_RATE = fpsRate;
}

void GLRenderer::initOpenGL() {
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(windowWidth, windowHeight, "Scene", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1);

    if (!shaderManager->createProgramFromFile(shaderProgramName, vertexShader, fragmentShader)) {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << shaderManager->getLastError();
        return;
    }

    shaderProgramId = shaderManager->getIDFromName(shaderProgramName);
}

void GLRenderer::run(double deltaTime) {

    if (deltaTime > (lastDeltaTime + (CURRENNT_FPS_RATE / 30.0f * 0.015))) {

        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) == 0) {

            //std::cout << count << ": " << deltaTime << std::endl;
            //count++;

            // getCamera()->processKeyboardInput(deltaTime);
            currentScene->processKeyboardInput(deltaTime);
            currentScene->updateScene(deltaTime);

            float ratio;
            int width, height;
            glUseProgram(shaderProgramId);

            glfwGetFramebufferSize(window, &width, &height);
            ratio = width / (float)height;

            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            glEnable(GL_DEPTH_TEST);
            glCullFace(GL_BACK);

            //uniform vec4 eyeLocation;
            GLint eyeLocation_UL = glGetUniformLocation(shaderProgramId, "eyeLocation");
            glUniform4f(eyeLocation_UL,
                getCamera().gCameraEye.x,
                getCamera().gCameraEye.y,
                getCamera().gCameraEye.z,
                1.0f
            );

            glm::mat4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);
            glm::mat4 matView = getCamera().getViewMatrix();

            GLint matProjection_UL = glGetUniformLocation(shaderProgramId, "matProjection");
            glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

            GLint matView_UL = glGetUniformLocation(shaderProgramId, "matView");
            glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

            if (currentScene != nullptr) {
                for (unsigned int index = 0; index != currentScene->getObjectsToDraw().size(); index++) {
                    GameObject* mCurrentObj = currentScene->mObjectsToDraw[index];
                    if (mCurrentObj->bIsVisible) {
                        glm::mat4 matModel = glm::mat4(1.0f);
                        drawObject(mCurrentObj, matModel);
                    }
                }
            }

            PhysicsEngine::getInstance().updatePhysics(deltaTime);

            glfwSwapBuffers(window);
            glfwPollEvents();

            std::stringstream ssTitle;
            ssTitle << windowTitle << " -- "
                << "Camera (x,y,z): "
                << getCamera().gCameraEye.x << ", "
                << getCamera().gCameraEye.y << ", "
                << getCamera().gCameraEye.z << "), "
                << getCamera().position.x << ", "
                << getCamera().position.y << ", "
                << getCamera().position.z << ", Pitch: "
                << getCamera().pitch << ", Yaw: "
                << getCamera().yaw << "";

            std::string theTitle = ssTitle.str();

            glfwSetWindowTitle(window, theTitle.c_str());
        }

        lastDeltaTime = deltaTime;
    }

}

void GLRenderer::drawObject(GameObject* mCurrentObj, glm::mat4 matModelParent) {
    glm::mat4 matModel = matModelParent;

    // Translation
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        glm::vec3(mCurrentObj->drawPosition.x,
            mCurrentObj->drawPosition.y,
            mCurrentObj->drawPosition.z));

    // Now we are all bougie, using quaternions
    glm::mat4 matRotation = glm::mat4(mCurrentObj->getQuatOrientation());

    // Scaling matrix
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(mCurrentObj->drawScale.x,
            mCurrentObj->drawScale.y,
            mCurrentObj->drawScale.z));

    // Combine all these transformation
    matModel = matModel * matTranslate;
    matModel = matModel * matRotation;
    matModel = matModel * matScale;

    GLint matModel_UL = glGetUniformLocation(shaderProgramId, "matModel");
    glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));

    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));
    GLint matModel_IT_UL = glGetUniformLocation(shaderProgramId, "matModel_IT");
    glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));

    if (mCurrentObj->bIsWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramId, "bDoNotLight");

    if (mCurrentObj->bDoNotLight) {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }
    else {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_FALSE);
    }

    GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramId, "bUseDebugColour");

    if (mCurrentObj->bUseDebugColours) {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_TRUE);
        GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramId, "debugColourRGBA");
        glUniform4f(debugColourRGBA_UL,
            mCurrentObj->wholeObjectDebugColourRGBA.r,
            mCurrentObj->wholeObjectDebugColourRGBA.g,
            mCurrentObj->wholeObjectDebugColourRGBA.b,
            mCurrentObj->wholeObjectDebugColourRGBA.a);
    }
    else {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
    }

    ModelDrawInfo modelInfo;
    if (mGameObjectManager->findDrawInfoByModelName(mCurrentObj->getFileName(), modelInfo)) {
        glBindVertexArray(modelInfo.VAO_ID);
        glDrawElements(GL_TRIANGLES,
            modelInfo.NUM_OF_INDICES,
            GL_UNSIGNED_INT,
            0);
        glBindVertexArray(0);
    }

    glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
        glm::vec3(
            1.0f / mCurrentObj->drawScale.x,
            1.0f / mCurrentObj->drawScale.y,
            1.0f / mCurrentObj->drawScale.z
        ));

    matModel = matModel * matRemoveScaling;

    for (GameObject* pChild : mCurrentObj->vecChildMeshes) {
        drawObject(pChild, matModel);
    }

    return;
}

void GLRenderer::stop() {
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}

// Define a callback function for mouse input
void GLRenderer::mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    //if (camera != nullptr) {
    //    if (getCamera() != nullptr) {
    //        // Calculate the change in mouse position
    //        float xoffset = xpos - lastX;
    //        float yoffset = lastY - ypos;  // Reversed since y-coordinates range from bottom to top

    //        // Update lastX and lastY for the next frame
    //        lastX = xpos;
    //        lastY = ypos;

    //        // Adjust the camera's yaw and pitch based on mouse movement
    //        getCamera()->yaw += xoffset * (getCamera()->sensitivity - 0.3);
    //        getCamera()->pitch += yoffset * (getCamera()->sensitivity - 0.3);

    //        // Limit pitch to avoid flipping the camera
    //        if (getCamera()->pitch > 89.0f) getCamera()->pitch = 89.0f;
    //        if (getCamera()->pitch < -89.0f) getCamera()->pitch = -89.0f;

    //        // Update the camera's front vector
    //        getCamera()->updateCameraVectors();
    //    }
    //}
}