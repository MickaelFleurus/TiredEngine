#pragma once

#include <filesystem>
#include <memory>

namespace Core {
class CEngineLoop;
}

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Renderer {
class CTextureManager;
class CSpriteManager;
} // namespace Renderer

namespace YAML {
class Node;
} // namespace YAML

namespace Scene {
class CAbstractScene;
class CSceneLoader {
public:
    CSceneLoader(Core::CEngineLoop& engineLoop,
                 Utils::CFileHandler& fileHandler);
    virtual ~CSceneLoader();

    void LoadSceneFromFile(const std::filesystem::path& path);
    virtual std::unique_ptr<CAbstractScene>
    LoadGameSpecificScenePart(const YAML::Node& sceneData) = 0;

private:
    Core::CEngineLoop& mEngineLoop;
    Utils::CFileHandler& mFileHandler;
};
} // namespace Scene
