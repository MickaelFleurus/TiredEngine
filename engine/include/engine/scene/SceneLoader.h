#pragma once

#include <string>

namespace Core {
class CEngineLoop;
}

namespace Utils {
class CFileHandler;
} // namespace Utils

namespace Scene {
class CSceneLoader {
public:
    CSceneLoader(Core::CEngineLoop& engineLoop,
                 Utils::CFileHandler& fileHandler);
    ~CSceneLoader() = default;

    void LoadSceneFromFile(const std::string& filePath);

private:
    Core::CEngineLoop& mEngineLoop;
    Utils::CFileHandler& mFileHandler;
};
} // namespace Scene
