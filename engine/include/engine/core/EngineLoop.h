#pragma once

#include "engine/core/Inputs.h"
#include "engine/debug/OverlordManager.h"
#include "engine/input/InputHandler.h"
#include "engine/renderer/ShaderFactory.h"
#include "engine/renderer/Window.h"

#include "engine/component/ComponentManager.h"
#include "engine/material/MaterialFactory.h"

#include "engine/font/FontHandler.h"
#include "engine/utils/FileHandler.h"

#include "engine/renderer/TextureManager.h"

#include <chrono>
#include <expected>

namespace Scene {
class CAbstractScene;
class ISceneHandler;
} // namespace Scene

namespace System {
class CSystem;
}

namespace Core {
class CEngineLoop {
public:
    CEngineLoop(System::CSystem& system);
    virtual ~CEngineLoop();

    std::expected<void, const char*> Initialize();

    void SetCurrentScene(std::unique_ptr<Scene::CAbstractScene>&& scene);
    Scene::CAbstractScene* GetCurrentScene() const;

    bool Run();

    virtual void GameLoop(float deltaTime) = 0;

protected:
    Renderer::CWindow mWindow;
    Debug::COverlordManager mOverlordManager;
    CInputs mInputs;
    Input::CInputHandler mInputHandler;
    Component::CComponentManager mComponentManager;
    std::unique_ptr<Scene::CAbstractScene> mCurrentScene;
    std::chrono::time_point<std::chrono::high_resolution_clock> mLastFrameTime;

    Renderer::CTextureManager mTextureManager;
    Material::CMaterialFactory mMaterialFactory;
    Font::CFontHandler mFontHandler;
};
} // namespace Core