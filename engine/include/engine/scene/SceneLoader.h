#pragma once

#include <memory>
#include <string>

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
                 Utils::CFileHandler& fileHandler,
                 Renderer::CTextureManager& textureManager,
                 Renderer::CSpriteManager& spriteManager);
    virtual ~CSceneLoader();

    void LoadSceneFromFile(const std::string& filePath);
    virtual std::unique_ptr<CAbstractScene>
    LoadGameSpecificScenePart(const YAML::Node& sceneData) = 0;

private:
    Core::CEngineLoop& mEngineLoop;
    Utils::CFileHandler& mFileHandler;
    Renderer::CTextureManager& mTextureManager;
    Renderer::CSpriteManager& mSpriteManager;
};
} // namespace Scene
