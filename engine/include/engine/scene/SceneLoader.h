#pragma once

#include <filesystem>
#include <unordered_map>
#include <utility>

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
    bool SaveCurrentScene() const;

private:
    Core::CEngineLoop& mEngineLoop;
    Utils::CFileHandler& mFileHandler;
    Core::CEntityManager& mEntityManager;
    Component::CManager& mComponentManager;
    std::filesystem::path mCurrentScenePath;
    std::unordered_map<uint32_t, std::pair<uint32_t, nlohmann::json>>
        mOriginalEntityData;
};
} // namespace Scene
