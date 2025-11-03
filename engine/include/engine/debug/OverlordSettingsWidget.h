#pragma once

#include <optional>
#include <string>

#include "engine/debug/IOverlordItem.h"

namespace Utils {
class CFileHandler;
}

namespace Scene {
class ISceneHandler;
}

namespace Debug {
class COverlordSettingsWidget : public IOverlordItem {
public:
    COverlordSettingsWidget(Utils::CFileHandler& fileHandler,
                            Scene::ISceneHandler& sceneHandler);

    void Render() override;
    const char* GetName() const override;

private:
    Utils::CFileHandler& mFileHandler;
    Scene::ISceneHandler& mSceneHandler;

    std::optional<std::string> mDefaultSceneToLoad;
};
} // namespace Debug