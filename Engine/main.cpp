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

int main() {
	std::cout << "Please wait, Starting the scene..." << std::endl;
    GameEngine engine = GameEngine();
    engine.setFPSRate(FPS_30);

    int shaderProgramId = engine.getShaderProgramId();
   
    SceneLevel1 scene(engine.getShaderProgramId(), "scene_1.json");
    BaseScene* baseScenePtr = &scene;

    engine.setCurrentScene(&scene);
    engine.init();
    engine.runGameLoop();
    engine.stop();

    return 0;
}