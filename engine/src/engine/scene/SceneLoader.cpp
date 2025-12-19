#include "engine/scene/SceneLoader.h"

namespace Scene {
CSceneLoader::CSceneLoader(Core::CEngineLoop& engineLoop,
                           Utils::CFileHandler& fileHandler)
    : mEngineLoop(engineLoop), mFileHandler(fileHandler) {
}

void CSceneLoader::LoadSceneFromFile(const std::string& filePath) {
    if (!mFileHandler.DoesFileExist(filePath, ".yaml")) {
        LOG_ERROR("Scene file does not exist: {}", filePath);
        return;
    }
    auto yamlData = mFileHandler.LoadYAML(filePath);
}
} // namespace Scene
