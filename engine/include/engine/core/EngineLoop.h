#pragma once

#include "engine/core/Inputs.h"
#include "engine/debug/OverlordManager.h"
#include "engine/input/InputHandler.h"
#include "engine/renderer/ShaderFactory.h"
#include "engine/renderer/Window.h"

#include "engine/component/ComponentManager.h"
#include "engine/core/WindowData.h"

#include <chrono>
#include <expected>

namespace Scene {
class AbstractScene;
}

namespace Core {
class CEngineLoop {
public:
    CEngineLoop();
    virtual ~CEngineLoop();

    std::expected<void, const char*> EverythingInitialisedCorrectly() const;

    void StartScene(std::unique_ptr<Scene::AbstractScene>&& scene);

    bool Run();

    virtual void GameLoop(float deltaTime) = 0;

protected:
    Renderer::CWindow mWindow;
    Renderer::CShaderFactory mShaderFactory;
    Debug::COverlordManager mOverlordManager;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    std::unique_ptr<Scene::AbstractScene> mCurrentScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;

    Component::CComponentManager mComponentManager;
    Core::CWindowData mWindowData;
};
} // namespace Core