#pragma once

#include "BaseScene.h"
#include "../Core/OpenGlCommons.h"
#include "../Graphics/ShaderManager/ShaderManager.h"
#include "../Graphics/VAOManager/VAOManager.h"

#include <string>

enum FPS_RATE {
	FPS_30 = 30,
	FPS_45 = 45,
	FPS_60 = 60
};

class GLRenderer
{
public:

	GLRenderer();
	~GLRenderer();

	void run(double deltaTime);
	void stop();

	GLint getShaderProgramId();

	void setShadersBasePath(std::string basePath);

	void setCurrentScene(BaseScene* currentScene);
	void setVertexShader(std::string fileName);
	void setfragmentShader(std::string fileName);

	void setWindowWidth(int width);
	void setWindoHeight(int height);
	void setWindowTitle(std::string title);

	void setFPSRate(FPS_RATE fpsRate);

	void mouseCallback(GLFWwindow* window, double xpos, double ypos);

	Camera& getCamera();
	GLFWwindow* getWindow();
	
	VAOManager* mGameObjectManager = NULL;

	float lastX;
	float lastY;

	int count = 0;
	float lastDeltaTime;

	FPS_RATE CURRENNT_FPS_RATE = DEFAULT_FPS;

	const FPS_RATE DEFAULT_FPS = FPS_60;

private:
	GLFWwindow* window;
	void initOpenGL();

	int windowWidth = 1280;
	int windowHeight = 720;
	std::string windowTitle = "MyGame";

	BaseScene* currentScene;

	ShaderManager* shaderManager;
	Shader vertexShader = Shader("vertex_shader.glsl");
	Shader fragmentShader = Shader("fragment_shader.glsl");

	std::string shaderBasePath = "assets/shaders";
	std::string modelsBasePath = "assets/models";
	std::string shaderProgramName = "shader01";

	void drawObject(GameObject* mCurrentObj, glm::mat4 matModel);

	GLint shaderProgramId = 0;
};

