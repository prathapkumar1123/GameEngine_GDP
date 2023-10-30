#include "GameEngine.h"

#include <windows.h>
#include <locale>
#include <codecvt>
#include <fstream>

class GameEngine::GameEngineImpl {
public:
    GLRenderer* glRenderer;

    bool isGameLoopRunning = true;
    double lastTime;

    Scene* currentScene;

    std::string modelsBasePath = "/assets/models";
    std::string shadersBasePath = "assets/shaders";
    std::string scenesBasePath = "assets/scenes";

    std::vector<std::function<void(float)>> updateFunctions;
    std::vector<std::function<void()>> renderFunctions;

    void loadAllModels();

private:
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
    glRenderer->mGameObjectManager->loadModelIntoVAO(modelName, modeInfo, glRenderer->getShaderProgramId());
    std::cout << "Loaded: " << modeInfo.NUM_OF_VERTICES << " vertices" << std::endl;
}


GameEngine::GameEngine(): impl(new GameEngineImpl) {
    impl->glRenderer = new GLRenderer();
    impl->loadAllModels();
    impl->lastTime = glfwGetTime();
}

GameEngine::~GameEngine() {
	release();
}

void GameEngine::init() {
    impl->currentScene->loadScene();
}

void GameEngine::stop() {
    impl->glRenderer->stop();
}
 
void GameEngine::release() {
    delete impl->glRenderer;
    delete impl;
}

GLRenderer* GameEngine::getRenderer() {
    return impl->glRenderer;
}

int GameEngine::getShaderProgramId() {
    return impl->glRenderer->getShaderProgramId();
}

void GameEngine::runGameLoop() {
    while (impl->isGameLoopRunning && !glfwWindowShouldClose(impl->glRenderer->getWindow())) {
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - impl->lastTime;

        impl->glRenderer->run(deltaTime);

        for (const auto& renderFunc : impl->renderFunctions) {
            renderFunc();
        }

        for (const auto& updateFunc: impl->updateFunctions) {
            updateFunc(deltaTime);
        }
    }
}

void GameEngine::registerUpdateFunction(std::function<void(float)> updateFunc) {
    impl->updateFunctions.push_back(updateFunc);
}

void GameEngine::registerRenderFunction(std::function<void()> renderFunc) {
    impl->renderFunctions.push_back(renderFunc);
}

Scene* GameEngine::getCurrentScene() {
    return this->impl->currentScene;
}

GLFWwindow* GameEngine::getWindow() {
    return impl->glRenderer->getWindow();
}

void GameEngine::setCurrentScene(Scene* scene) {
    impl->glRenderer->setCurrentScene(scene);
    impl->currentScene = scene;
}

void GameEngine::setAssetsPath(std::string modelsBasePath, std::string shadersBasePath, std::string scenesBasePath) {
    impl->modelsBasePath = std::string("..\\") + modelsBasePath + std::string("\\");
    impl->shadersBasePath = std::string("..\\") + shadersBasePath + std::string("\\");
    impl->scenesBasePath = std::string("..\\") + scenesBasePath + std::string("\\");
}