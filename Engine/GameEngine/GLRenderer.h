#pragma once

#include "Scene.h"
#include "../Core/OpenGlCommons.h"
#include "../Graphics/ShaderManager/ShaderManager.h"
#include "../Graphics/VAOManager/VAOManager.h"

#include <string>

class GLRenderer
{
public:
	GLRenderer();
	~GLRenderer();

	void run(double deltaTime);
	void stop();

	GLint getShaderProgramId();

	void setShadersBasePath(std::string basePath);

	void setCurrentScene(Scene* currentScene);
	void setVertexShader(std::string fileName);
	void setfragmentShader(std::string fileName);

	void setWindowWidth(int width);
	void setWindoHeight(int height);
	void setWindowTitle(std::string title);

	void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	Camera& getCamera();
	GLFWwindow* getWindow();
	
	VAOManager* mGameObjectManager = NULL;

	float lastX;
	float lastY;

private:
	GLFWwindow* window;
	void initOpenGL();

	int windowWidth = 1280;
	int windowHeight = 720;
	std::string windowTitle = "MyGame";

	Scene* currentScene;

	ShaderManager* shaderManager;
	Shader vertexShader = Shader("vertex_shader.glsl");
	Shader fragmentShader = Shader("fragment_shader.glsl");

	std::string shaderBasePath = "assets/shaders";
	std::string modelsBasePath = "assets/models";
	std::string shaderProgramName = "shader01";

	void drawObject(GameObject* mCurrentObj, glm::mat4 matModel);

	GLint shaderProgramId = 0;
};

