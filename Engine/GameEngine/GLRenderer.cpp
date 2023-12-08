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
    delete mGameObjectManager;
    delete shaderManager;
}

GLFWwindow* GLRenderer::getWindow() {
	return window;
}

void GLRenderer::setVertexShader(std::string name) {
	vertexShader.setShaderFileName(name);
}

void GLRenderer::setFragmentShader(std::string name) {
	fragmentShader.setShaderFileName(name);
}

void GLRenderer::setGeometryShader(std::string name) {
    geometryShader.setShaderFileName(name);
}

void GLRenderer::setCurrentScene(BaseScene* currentScene) {
    std::cout << "Current Scene Setting..." << std::endl;
    this->currentScene = currentScene;
    this->mTextureManager = currentScene->textureManager;

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

void GLRenderer::setWindowHeight(int height) {
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
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    if (!shaderManager->createProgramFromFile(shaderProgramName, vertexShader, geometryShader, fragmentShader)) {
        std::cout << "Error: Couldn't compile or link:" << std::endl;
        std::cout << shaderManager->getLastError();
        return;
    }

    shaderProgramId = shaderManager->getIDFromName(shaderProgramName);
    mGameObjectManager->setShaderProgramId(shaderProgramId);
}

void GLRenderer::run(double deltaTime) {
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) == 0 && currentScene != nullptr) {

        currentScene->processKeyboardInput(deltaTime);
        currentScene->updateScene(deltaTime);

        float ratio;
        int width, height;
        glUseProgram(shaderProgramId);

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float)height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        glEnable(GL_DEPTH_TEST);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //// stencil testing
        //glEnable(GL_STENCIL_TEST);
        //// keep fragments if either stencil or depth fails, replace if both pass
        //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);

        //uniform vec4 eyeLocation;
        GLint eyeLocation_UL = glGetUniformLocation(shaderProgramId, "eyeLocation");
        glUniform4f(eyeLocation_UL,
            getCamera().gCameraEye.x,
            getCamera().gCameraEye.y,
            getCamera().gCameraEye.z,
            1.0f
        );

        glm::mat4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 10'000.0f);
        glm::mat4 matView = getCamera().getViewMatrix();

        GLint matProjection_UL = glGetUniformLocation(shaderProgramId, "matProjection");
        glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

        GLint matView_UL = glGetUniformLocation(shaderProgramId, "matView");
        glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

        removeDestroyedObjects();

        if (currentScene != nullptr) {
            for (unsigned int index = 0; index != currentScene->getObjectsToDraw().size(); index++) {
                GameObject* mCurrentObj = currentScene->mObjectsToDraw[index];
                if (mCurrentObj->bIsVisible && !mCurrentObj->isOutOfBounds) {
                    glm::mat4 matModel = glm::mat4(1.0f);
                    drawObject(mCurrentObj, matModel);
                }
            }
        }

        {
            // HACK: I'm making this here, but hey...
            GameObject* theSkyBox = new GameObject("Sphere_xyz_n_rgba_uv.ply");
            theSkyBox->simpleName = "skybox";
            theSkyBox->setUniformDrawScale(5000.0f);
            theSkyBox->setDrawPosition(Camera::getInstance().position);
            theSkyBox->bIsVisible = true;
            theSkyBox->bIsWireframe = false;
            theSkyBox->bDoNotLight = false;
            theSkyBox->inverse_mass = 0.0f;

            Mesh* mesh = new Mesh("SunnyDay");
            theSkyBox->setMesh(mesh);

            GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgramId, "bIsSkyBox");
            glUniform1f(bIsSkyBox_UL, (GLfloat) GL_TRUE);
            glCullFace(GL_FRONT);

            drawObject(theSkyBox, glm::mat4(1.0f));

            glUniform1f(bIsSkyBox_UL, (GLfloat)GL_FALSE);
            glCullFace(GL_BACK);
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

    glUniform1f(bDoNotLight_UL, (GLfloat) GL_FALSE);

    if (mCurrentObj->bDoNotLight) {
        glUniform1f(bDoNotLight_UL, (GLfloat)GL_TRUE);
    }

    GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramId, "bUseDebugColour");

    if (mCurrentObj->bUseDebugColors) {
        glUniform1f(bUseDebugColour_UL, (GLfloat) GL_TRUE);
        GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramId, "debugColourRGBA");
        glUniform4f(debugColourRGBA_UL,
            mCurrentObj->wholeObjectDebugColourRGBA.r,
            mCurrentObj->wholeObjectDebugColourRGBA.g,
            mCurrentObj->wholeObjectDebugColourRGBA.b,
            mCurrentObj->wholeObjectDebugColourRGBA.a);
    }
    else {
        glUniform1f(bUseDebugColour_UL, (GLfloat) GL_FALSE);
    }

    GLint bUseVertexColors_UL = glGetUniformLocation(shaderProgramId, "bUseVertexColors");
    glUniform1f(bUseVertexColors_UL, (GLfloat) GL_TRUE);

    if (mCurrentObj->getMesh() != nullptr) {
        glUniform1f(bUseVertexColors_UL, (GLfloat) GL_FALSE);
        this->setupTextures(mCurrentObj->getMesh());
    }
    else {
        glUniform1f(bUseVertexColors_UL, (GLfloat) GL_TRUE);
    }

    GLint bUseHeightMap_UL = glGetUniformLocation(shaderProgramId, "bUseHeightMap");
    glUniform1f(bUseHeightMap_UL, (GLfloat) GL_FALSE);

    GLint bUseDiscardMaskTexture_UL = glGetUniformLocation(shaderProgramId, "bUseDiscardMaskTexture");
    glUniform1f(bUseDiscardMaskTexture_UL, (GLfloat) GL_FALSE);

    //GLint bIsSkyBox_UL = glGetUniformLocation(shaderProgramId, "bIsSkyBox");
    //glUniform1f(bIsSkyBox_UL, (GLfloat) GL_FALSE);

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

void GLRenderer::setupTextures(Mesh* objMesh) {

    {
        GLint textureUnitNumber = 0;
        GLuint Texture00 = mTextureManager->getTextureIDFromName(objMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture00);
        GLint texture_00_UL = glGetUniformLocation(shaderProgramId, "texture_00");
        glUniform1i(texture_00_UL, textureUnitNumber);
    }

    {
        GLint textureUnitNumber = 1;
        GLuint Texture01 = mTextureManager->getTextureIDFromName(objMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture01);
        GLint texture_01_UL = glGetUniformLocation(shaderProgramId, "texture_01");
        glUniform1i(texture_01_UL, textureUnitNumber);
    }

    {
        GLint textureUnitNumber = 2;
        GLuint Texture02 = mTextureManager->getTextureIDFromName(objMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture02);
        GLint texture_02_UL = glGetUniformLocation(shaderProgramId, "texture_02");
        glUniform1i(texture_02_UL, textureUnitNumber);
    }

    {
        GLint textureUnitNumber = 3;
        GLuint Texture03 = mTextureManager->getTextureIDFromName(objMesh->textureName[textureUnitNumber]);
        glActiveTexture(GL_TEXTURE0 + textureUnitNumber);
        glBindTexture(GL_TEXTURE_2D, Texture03);
        GLint texture_03_UL = glGetUniformLocation(shaderProgramId, "texture_03");
        glUniform1i(texture_03_UL, textureUnitNumber);
    }

    GLint textureMixRatio_0_3_UL = glGetUniformLocation(shaderProgramId, "textureMixRatio_0_3");

    glUniform4f(textureMixRatio_0_3_UL,
        objMesh->textureRatios[0],
        objMesh->textureRatios[1],
        objMesh->textureRatios[2],
        objMesh->textureRatios[3]);

    // Set up a skybox
    {
        // uniform samplerCube skyBoxTexture;		// Texture unit 30
        GLint textureUnit30 = 30;
        GLuint skyBoxID = mTextureManager->getTextureIDFromName("SunnyDay");
        glActiveTexture(GL_TEXTURE0 + textureUnit30);
        // NOTE: Binding is NOT to GL_TEXTURE_2D
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyBoxID);
        GLint skyBoxSampler_UL = glGetUniformLocation(shaderProgramId, "skyBoxTexture");
        glUniform1i(skyBoxSampler_UL, textureUnit30);
    }


    return;
}

void GLRenderer::removeDestroyedObjects() {
    currentScene->mObjectsToDraw.erase(std::remove_if(currentScene->mObjectsToDraw.begin(), currentScene->mObjectsToDraw.end(),
        [](GameObject* obj) { return obj->isOutOfBounds; }),
        currentScene->mObjectsToDraw.end());
}

void GLRenderer::stop() {
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}