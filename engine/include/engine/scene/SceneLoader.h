#pragma once

#include <filesystem>

#include <nlohmann/json.hpp>

namespace Core {
class CEngineLoop;
class CEntityManager;
} // namespace Core

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Renderer {
class CTextureManager;
class CSpriteManager;
} // namespace Renderer

namespace Component {
class CManager;
} // namespace Component

namespace Scene {
class CAbstractScene;
class CSceneLoader {
public:
    CSceneLoader(Core::CEngineLoop& engineLoop,
                 Utils::CFileHandler& fileHandler,
                 Core::CEntityManager& entityManager,
                 Component::CManager& componentManager);
    virtual ~CSceneLoader() = default;

    void LoadSceneFromFile(const std::filesystem::path& path);

private:
    Core::CEngineLoop& mEngineLoop;
    Utils::CFileHandler& mFileHandler;
    Core::CEntityManager& mEntityManager;
    Component::CManager& mComponentManager;
};
} // namespace Scene
