#pragma once

#include <string>

namespace Scene {
class ISceneHandler;
}

namespace Utils {

class CFileHandler;

class COverlordSettings {
public:
    COverlordSettings(CFileHandler& fileHandler,
                      Scene::ISceneHandler& sceneHandler);

    void LoadSettings();
    void SaveSettings() const;

    const std::string& GetDefaultSceneToLoad() const;

private:
    void ApplySettings();

    CFileHandler& mFileHandler;
    Scene::ISceneHandler& mSceneHandler;

    std::string mDefaultSceneToLoad;
};
} // namespace Utils