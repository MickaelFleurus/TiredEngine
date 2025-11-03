#include "engine/utils/OverlordSettings.h"

#include "engine/scene/ISceneHandler.h"
#include "engine/utils/FileHandler.h"
#include <nlohmann/json.hpp>

namespace {
constexpr const char* kSettingsFile = "overlord_settings.json";

constexpr const char* kDefaultSceneKey = "default_scene";
} // namespace

namespace Utils {

COverlordSettings::COverlordSettings(CFileHandler& fileHandler,
                                     Scene::ISceneHandler& sceneHandler)
    : mFileHandler(fileHandler), mSceneHandler(sceneHandler) {
    LoadSettings();
    ApplySettings();
}

void COverlordSettings::LoadSettings() {
    const auto config = mFileHandler.LoadJson(kSettingsFile);
    config.at("default_scene").get_to(mDefaultSceneToLoad);
}

void COverlordSettings::SaveSettings() const {
    nlohmann::json config;
    config[kDefaultSceneKey] = mDefaultSceneToLoad;
    mFileHandler.SaveJson(kSettingsFile, config);
}

void COverlordSettings::ApplySettings() {
    mSceneHandler.CreateAndSetScene(mDefaultSceneToLoad);
}

const std::string& COverlordSettings::GetDefaultSceneToLoad() const {
    return mDefaultSceneToLoad;
}

} // namespace Utils