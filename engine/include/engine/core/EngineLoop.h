#pragma once

#include "engine/core/Inputs.h"
#include "engine/debug/OverlordManager.h"
#include "engine/input/InputHandler.h"
#include "engine/renderer/ShaderFactory.h"
#include "engine/renderer/Window.h"

#include "engine/component/ComponentManager.h"
#include "engine/core/WindowData.h"

#include "engine/scene/SceneHandler.h"

#include <chrono>
#include <expected>

namespace Scene {
class CAbstractScene;
}

namespace Core {
class CEngineLoop {
public:
    CEngineLoop();
    virtual ~CEngineLoop();

    std::expected<void, const char*> EverythingInitialisedCorrectly() const;

    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);
    Scene::CAbstractScene* GetCurrentScene() const;

    bool Run();

    virtual void GameLoop(float deltaTime) = 0;

protected:
    Scene::CSceneHandler& GetSceneHandler();

    Renderer::CWindow mWindow;
    Renderer::CShaderFactory mShaderFactory;
    Debug::COverlordManager mOverlordManager;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    std::unique_ptr<Scene::CAbstractScene> mCurrentScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;

    Component::CComponentManager mComponentManager;
    Core::CWindowData mWindowData;
    Scene::CSceneHandler mSceneLoader;
};
} // namespace Core