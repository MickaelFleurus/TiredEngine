#pragma once

#include <engine/debug/BufferWidget.h>
#include <engine/debug/EntityWidget.h>
#include <engine/debug/LevelSceneLoaderWidget.h>
#include <engine/debug/OverlordSettingsWidget.h>
#include <engine/debug/SceneHierarchy.h>
#include <engine/debug/SceneLoaderOverlord.h>
#include <engine/debug/TexturesWidget.h>
#include <engine/utils/OverlordSettings.h>
#include <engine/utils/Token.h>

namespace Utils {
class CFileHandler;
}

namespace Scene {
class ISceneHandler;
class CSceneLoader;
} // namespace Scene

namespace Component {
class CComponentManager;
} // namespace Component

namespace Font {
class CFontHandler;
} // namespace Font

namespace Vulkan {
class CBufferHandler;
} // namespace Vulkan

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace Debug {
class COverlordToolHandler {
public:
    COverlordToolHandler(Component::CComponentManager& componentManager,
                         Utils::CFileHandler& fileHandler,
                         Scene::ISceneHandler& sceneLoader,
                         Font::CFontHandler& fontHandler,
                         Vulkan::CBufferHandler& bufferHandler,
                         Renderer::CTextureManager& textureManager,
                         Scene::CSceneLoader& sceneLoaderOverlord);

    void Initialize();

    void RegisterTools();

private:
    Utils::COverlordSettings mSettings;

    COverlordSettingsWidget mOverlordSettings;
    CSceneLoaderOverlord mSceneLoaderOverlord;
    CEntityWidget mEntityWidget;
    CSceneHierarchy mSceneHierarchy;
    CBufferWidget mBufferWidget;
    CTexturesWidget mTexturesWidget;
    CLevelSceneLoaderWidget mLevelSceneLoaderWidget;

    CToken mToken;
};
} // namespace Debug
