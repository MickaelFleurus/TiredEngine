#pragma once

#include <optional>
#include <string>
#include <vector>

#include "engine/debug/IOverlordItem.h"

namespace Utils {
class CFileHandler;
class COverlordSettings;
} // namespace Utils

namespace Scene {
class ISceneHandler;
}

namespace Debug {
class COverlordSettingsWidget : public IOverlordItem {
public:
    COverlordSettingsWidget(Utils::COverlordSettings& settings,
                            Utils::CFileHandler& fileHandler,
                            Scene::ISceneHandler& sceneHandler);

    void Render() override;
    const char* GetName() const override;

private:
    Utils::COverlordSettings& mSettings;
    Utils::CFileHandler& mFileHandler;
    Scene::ISceneHandler& mSceneHandler;
    const std::vector<std::string> mSceneNames;

    bool mShowWidget = false;
    bool mHasDefaultSceneToLoad;
    std::string mSelectedDefaultScene;
};
} // namespace Debug