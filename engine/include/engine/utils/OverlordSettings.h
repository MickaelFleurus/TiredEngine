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

    bool HasDefaultSceneToLoad() const;
    const std::string& GetDefaultSceneToLoad() const;
    void SetDefaultSceneToLoad(const std::string& sceneName);

private:
    void ApplySettings();

    CFileHandler& mFileHandler;
    Scene::ISceneHandler& mSceneHandler;

    std::string mDefaultSceneToLoad;
};
} // namespace Utils