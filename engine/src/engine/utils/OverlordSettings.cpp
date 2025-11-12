#include "engine/utils/OverlordSettings.h"

#include "engine/scene/ISceneHandler.h"
#include "engine/utils/FileHandler.h"
#include <nlohmann/json.hpp>

namespace {
constexpr const char* kSettingsFile = "overlord_settings";

constexpr const char* kDefaultSceneKey = "default_scene";
} // namespace

namespace Utils {

COverlordSettings::COverlordSettings(CFileHandler& fileHandler,
                                     Scene::ISceneHandler& sceneHandler)
    : mFileHandler(fileHandler), mSceneHandler(sceneHandler) {
}

bool COverlordSettings::Initialize() {
    mPath = mFileHandler.GetTempFolder() + kSettingsFile;
    if (mFileHandler.DoesFileExist(mPath, ".json")) {
        LoadSettings();
        ApplySettings();
    }
    return true;
}

void COverlordSettings::LoadSettings() {
    const auto config = mFileHandler.LoadJson(mPath);
    config.at("default_scene").get_to(mDefaultSceneToLoad);
}

void COverlordSettings::SaveSettings() const {
    nlohmann::json config;
    config[kDefaultSceneKey] = mDefaultSceneToLoad;
    mFileHandler.SaveJson(mPath, config);
}

void COverlordSettings::ApplySettings() {
    if (!mDefaultSceneToLoad.empty()) {
        mSceneHandler.CreateAndSetScene(mDefaultSceneToLoad);
    }
}

bool COverlordSettings::HasDefaultSceneToLoad() const {
    return !mDefaultSceneToLoad.empty();
}

const std::string& COverlordSettings::GetDefaultSceneToLoad() const {
    return mDefaultSceneToLoad;
}

void COverlordSettings::SetDefaultSceneToLoad(const std::string& sceneName) {
    mDefaultSceneToLoad = sceneName;
}

} // namespace Utils