#pragma once
#include "core/Inputs.h"
#include "debug/Overlord.h"
#include "input/InputHandler.h"
#include "renderer/ShaderFactory.h"
#include "renderer/Window.h"

#include <expected>

namespace Scene {
class AbstractScene;
}

namespace Core {
class CGameLoop {
public:
    CGameLoop();
    std::expected<void, const char*> EverythingInitialisedCorrectly() const;

    bool Run();

private:
    Renderer::CWindow mWindow;
    Renderer::CShaderFactory mShaderFactory;
    Debug::COverlord mOverlord;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    std::unique_ptr<Scene::AbstractScene> mCurrentScene;
};
} // namespace Core