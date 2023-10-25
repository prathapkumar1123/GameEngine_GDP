#include "Globals/OpenGlCommons.h"
#include "VAOManager/VAOManager.h"
#include "ShaderManager/ShaderManager.h"
#include "GLFW_Callbacks.h"
#include "Mesh.h"

#include <iostream>

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

// glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>

#include <stdlib.h>
#include <stdio.h>

glm::vec3 g_cameraEye = glm::vec3(0.0, 70.0, 181.0f);
glm::vec3 g_cameraTarget = glm::vec3(0.0f, 5.0f, 0.0f);
glm::vec3 g_upVector = glm::vec3(0.0f, 1.0f, 0.0f);

VAOManager* mMeshManager = NULL;

std::vector<Mesh*> mMeshesToDraw;

void DrawObject(Mesh* pCurrentMesh, glm::mat4 matModel, GLuint shaderProgramID);

static void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}

int main() {

	std::cout << "Please wait, Starting the scene..." << std::endl;

	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);
	
	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	window = glfwCreateWindow(640, 480, "Scene", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    glfwSetKeyCallback(window, key_callback);

	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	ShaderManager* shaderManager = new ShaderManager();
	shaderManager->setBasePath("shaders");

	Shader vertexShader;
	vertexShader.fileName = "vertex_shader.glsl";

	Shader fragmentShader;
	fragmentShader.fileName = "fragment_shader.glsl";

	if (!shaderManager->createProgramFromFile("shader01", vertexShader, fragmentShader)) {
		std::cout << "Error: Couldn't compile or link:" << std::endl;
		std::cout << shaderManager->getLastError();
		return -1;
	}

	GLuint shaderProgramID = shaderManager->getIDFromName("shader01");

	::mMeshManager = new VAOManager();
	::mMeshManager->setBasePath("assets/models");

	ModelDrawInfo bannerDrawingInfo;
	::mMeshManager->loadModelIntoVAO("banner.ply", bannerDrawingInfo, shaderProgramID);
	std::cout << "Loaded: " << bannerDrawingInfo.NUM_OF_VERTICES << " vertices" << std::endl;

    Mesh* pBanner = new Mesh("banner.ply");
    pBanner->simpleName = "banner";
    pBanner->setUniformDrawScale(50.0f);
    pBanner->bDoNotLight = true;
    pBanner->drawPosition = glm::vec3(0.0f, -30.0f, 0.0f);

    ::mMeshesToDraw.push_back(pBanner);

	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{

		float ratio;
		int width, height;

		glUseProgram(shaderProgramID);

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float) height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// While drawing a pixel, see if the pixel that's already there is closer or not?
		glEnable(GL_DEPTH_TEST);
		// (Usually) the default - does NOT draw "back facing" triangles
		glCullFace(GL_BACK);

		//uniform vec4 eyeLocation;
		GLint eyeLocation_UL = glGetUniformLocation(shaderProgramID, "eyeLocation");
		glUniform4f(eyeLocation_UL, ::g_cameraEye.x, ::g_cameraEye.y, ::g_cameraEye.z, 1.0f);

		glm::mat4 matProjection = glm::perspective(0.6f, ratio, 0.1f, 1000.0f);

		glm::mat4 matView = glm::lookAt(::g_cameraEye, ::g_cameraTarget, ::g_upVector);

		GLint matProjection_UL = glGetUniformLocation(shaderProgramID, "matProjection");
		glUniformMatrix4fv(matProjection_UL, 1, GL_FALSE, glm::value_ptr(matProjection));

		GLint matView_UL = glGetUniformLocation(shaderProgramID, "matView");
		glUniformMatrix4fv(matView_UL, 1, GL_FALSE, glm::value_ptr(matView));

		for (unsigned int index = 0; index != ::mMeshesToDraw.size(); index++)
		{
			Mesh* pCurrentMesh = ::mMeshesToDraw[index];
			if (pCurrentMesh->bIsVisible)
			{
				glm::mat4 matModel = glm::mat4(1.0f);   // Identity matrix
				DrawObject(pCurrentMesh, matModel, shaderProgramID);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void DrawObject(Mesh* pCurrentMesh, glm::mat4 matModelParent, GLuint shaderProgramID) {

    // mat4x4_identity(m);
    glm::mat4 matModel = matModelParent;

    // Translation
    glm::mat4 matTranslate = glm::translate(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawPosition.x,
            pCurrentMesh->drawPosition.y,
            pCurrentMesh->drawPosition.z));

    // Now we are all bougie, using quaternions
    glm::mat4 matRotation = glm::mat4(pCurrentMesh->get_qOrientation());


    // Scaling matrix
    glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
        glm::vec3(pCurrentMesh->drawScale.x,
            pCurrentMesh->drawScale.y,
            pCurrentMesh->drawScale.z));
    //--------------------------------------------------------------

    // Combine all these transformation
    matModel = matModel * matTranslate;         // Done last
    matModel = matModel * matRotation;
    matModel = matModel * matScale;             // Mathematically done 1st

    GLint matModel_UL = glGetUniformLocation(shaderProgramID, "matModel");
    glUniformMatrix4fv(matModel_UL, 1, GL_FALSE, glm::value_ptr(matModel));

    // Also calculate and pass the "inverse transpose" for the model matrix
    glm::mat4 matModel_InverseTranspose = glm::inverse(glm::transpose(matModel));

    // uniform mat4 matModel_IT;
    GLint matModel_IT_UL = glGetUniformLocation(shaderProgramID, "matModel_IT");
    glUniformMatrix4fv(matModel_IT_UL, 1, GL_FALSE, glm::value_ptr(matModel_InverseTranspose));


    if (pCurrentMesh->bIsWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    GLint bDoNotLight_UL = glGetUniformLocation(shaderProgramID, "bDoNotLight");

    if (pCurrentMesh->bDoNotLight) {
        glUniform1f(bDoNotLight_UL, (GLfloat) GL_TRUE);
    }
    else {
        glUniform1f(bDoNotLight_UL, (GLfloat) GL_FALSE);
    }

    //uniform bool bUseDebugColour;	
    GLint bUseDebugColour_UL = glGetUniformLocation(shaderProgramID, "bUseDebugColour");

    if (pCurrentMesh->bUseDebugColours) {
        glUniform1f(bUseDebugColour_UL, (GLfloat) GL_TRUE);
        GLint debugColourRGBA_UL = glGetUniformLocation(shaderProgramID, "debugColourRGBA");
        glUniform4f(debugColourRGBA_UL,
            pCurrentMesh->wholeObjectDebugColourRGBA.r,
            pCurrentMesh->wholeObjectDebugColourRGBA.g,
            pCurrentMesh->wholeObjectDebugColourRGBA.b,
            pCurrentMesh->wholeObjectDebugColourRGBA.a);
    }
    else {
        glUniform1f(bUseDebugColour_UL, (GLfloat)GL_FALSE);
    }


    ModelDrawInfo modelInfo;
    if (::g_pMeshManager->findDrawInfoByModelName(pCurrentMesh->getMeshName(), modelInfo)) {
        // Found it!!!

        glBindVertexArray(modelInfo.VAO_ID); 		//  enable VAO (and everything else)
        glDrawElements(GL_TRIANGLES,
            modelInfo.NUM_OF_INDICES,
            GL_UNSIGNED_INT,
            0);
        glBindVertexArray(0); 			            // disable VAO (and everything else)
    }

    /*glm::mat4 matRemoveScaling = glm::scale(glm::mat4(1.0f),
        glm::vec3(
            1.0f / pCurrentMesh->drawScale.x,
            1.0f / pCurrentMesh->drawScale.y,
            1.0f / pCurrentMesh->drawScale.z
        ));

    matModel = matModel * matRemoveScaling;*/

    for (Mesh* pChild : pCurrentMesh->vec_pChildMeshes) {

        // Notice we are passing the "parent" (already transformed) matrix
        // NOT an identiy matrix

        // if you are using scaling, you can "undo" the scaling
        // i.e. add the opposite of the scale the parent had

        DrawObject(pChild, matModel, shaderProgramID);

    }

    return;
}