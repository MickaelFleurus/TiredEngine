#pragma once
#include "engine/core/Inputs.h"
#include "engine/debug/Overlord.h"
#include "engine/input/InputHandler.h"
#include "engine/renderer/ShaderFactory.h"
#include "engine/renderer/Window.h"

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

    template <typename T, typename... Args>
    void LoadScene(Args&&... args) {
        mCurrentScene = std::make_unique<T>(std::forward<Args>(args)...);
    }

    bool Run();

    virtual void GameLoop(float deltaTime) = 0;

protected:
    Renderer::CWindow mWindow;
    Renderer::CShaderFactory mShaderFactory;
    Debug::COverlord mOverlord;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    std::unique_ptr<Scene::AbstractScene> mCurrentScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;
};
} // namespace Core