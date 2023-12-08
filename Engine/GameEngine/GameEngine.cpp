#include "GameEngine.h"
#include "HRTimer.h"
#include "../Game/SceneLevel1.h"

#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>

class GameEngine::GameEngineImpl {
public:
    GLRenderer* glRenderer;

    bool isGameLoopRunning = true;
    double lastTime = 0.0f;

    void setCurrentScene(BaseScene* scene);
    BaseScene* getCurrentScene();

    std::string modelsBasePath = "/assets/models";
    std::string shadersBasePath = "assets/shaders";
    std::string scenesBasePath = "assets/scenes";

    std::vector<std::function<void(float)>> updateFunctions;
    std::vector<std::function<void()>> renderFunctions;

    void loadAllModels();

    void reload();

    HRTimer* timer;

private:
    BaseScene* currentScene;
    void loadModelIntoVAO(std::string modelName);
};

void GameEngine::GameEngineImpl::loadAllModels() {
    WIN32_FIND_DATA findFileData;

    wchar_t buffer[MAX_PATH];
    std::wstring modelsCompletePath;
    if (_wgetcwd(buffer, MAX_PATH) != nullptr) {
        modelsCompletePath = buffer + std::wstring(modelsBasePath.begin(), modelsBasePath.end());
    }

    HANDLE hFind = FindFirstFile((std::wstring(modelsCompletePath) + L"\\*").c_str(), &findFileData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                std::wstring wideFileName = findFileData.cFileName;
                std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
                std::string narrowFileName = converter.to_bytes(wideFileName);

                loadModelIntoVAO(narrowFileName);
            }
        } while (FindNextFileW(hFind, &findFileData) != 0);

        FindClose(hFind);
    }
    else {
        std::wcerr << L"Could not open the directory." << std::endl;
    }
}

void GameEngine::GameEngineImpl::loadModelIntoVAO(std::string modelName) {
    ModelDrawInfo modeInfo;
    glRenderer->mGameObjectManager->loadModelIntoVAO(modelName, modeInfo);
}

void GameEngine::GameEngineImpl::reload() {

}

void GameEngine::GameEngineImpl::setCurrentScene(BaseScene* scene) {
    this->currentScene = scene;
    this->glRenderer->setCurrentScene(scene);
}

BaseScene* GameEngine::GameEngineImpl::getCurrentScene() {
    return this->currentScene;
}

GameEngine::GameEngine(): impl(new GameEngineImpl) {
    impl->glRenderer = new GLRenderer();
    impl->loadAllModels();
    impl->timer = new HRTimer(60);
}

GameEngine::~GameEngine() {
	release();
}

void GameEngine::init() {
    impl->getCurrentScene()->loadScene();
    impl->lastTime = glfwGetTime();
}

void GameEngine::stop() {
    impl->glRenderer->stop();
}
 
void GameEngine::release() {
    delete impl->glRenderer;
}

void GameEngine::setFPSRate(FPS_RATE fpsRate) {
    impl->glRenderer->setFPSRate(fpsRate);
}

GLRenderer* GameEngine::getRenderer() {
    return impl->glRenderer;
}

int GameEngine::getShaderProgramId() {
    return impl->glRenderer->getShaderProgramId();
}

VAOManager* GameEngine::getVAOManager() {
    return impl->glRenderer->mGameObjectManager;
}

void GameEngine::runGameLoop() {
    while (impl->isGameLoopRunning && !glfwWindowShouldClose(impl->glRenderer->getWindow())) {

        if (impl->getCurrentScene()->isSceneLoaded) {
            double deltaTime = impl->timer->getFrameTime();

            if (glfwGetKey(getWindow(), GLFW_KEY_R) == GLFW_PRESS) {
                getCurrentScene()->loadScene();
            }

            if (impl->glRenderer != nullptr)
                impl->glRenderer->run(deltaTime);


            /*for (const auto& renderFunc : impl->renderFunctions) {
                renderFunc();
            }

            for (const auto& updateFunc: impl->updateFunctions) {
                updateFunc(deltaTime);
            }*/
        }
    }
}

void GameEngine::registerUpdateFunction(std::function<void(float)> updateFunc) {
    impl->updateFunctions.push_back(updateFunc);
}

void GameEngine::registerRenderFunction(std::function<void()> renderFunc) {
    impl->renderFunctions.push_back(renderFunc);
}

BaseScene* GameEngine::getCurrentScene() {
    return this->impl->getCurrentScene();
}

GLFWwindow* GameEngine::getWindow() {
    return impl->glRenderer->getWindow();
}

void GameEngine::setCurrentScene(BaseScene* scene) {
    impl->glRenderer->setCurrentScene(scene);
    impl->setCurrentScene(scene);
    scene->setVAOManager(getVAOManager());
}

void GameEngine::setAssetsPath(std::string modelsBasePath, std::string shadersBasePath, std::string scenesBasePath) {
    impl->modelsBasePath = std::string("..\\") + modelsBasePath + std::string("\\");
    impl->shadersBasePath = std::string("..\\") + shadersBasePath + std::string("\\");
    impl->scenesBasePath = std::string("..\\") + scenesBasePath + std::string("\\");
}