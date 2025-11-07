#include "engine/system/System.h"

#include <format>

namespace {
constexpr const char* kSystemFile = "system_config";
}

namespace System {
CSystem::CSystem() {
}

Utils::CFileHandler& CSystem::GetFileHandler() {
    return mFileHandler;
}

const SDisplayParameter& CSystem::GetDisplayParameters() const {
    return mDisplayParameters;
}

const std::string& CSystem::GetGameName() const {
    return mGameName;
}

bool CSystem::Initialize() {
    auto json = mFileHandler.LoadJson(
        std::format("{}/{}", mFileHandler.GetAssetsFolder(), kSystemFile));
    if (json.is_null()) {
        return false;
    }
    mGameName = json["parameter"].value("gameName", "DefaultGameName");
    mFileHandler.CreateTempFolder(mGameName);

    {
        const auto& settings = json["settings"];
        if (settings.contains("resolution")) {
            const auto& resolution = settings["resolution"];
            mDisplayParameters.width = resolution.value("width", 800);
            mDisplayParameters.height = resolution.value("height", 600);
        }
        mDisplayParameters.fullscreen = settings.value("fullscreen", false);
        mDisplayParameters.vSync = settings.value("v_sync", false);
        if (settings.contains("fps_limit")) {
            mDisplayParameters.fpsLimit = settings["fps_limit"].get<int>();
        }
    }

    return true;
}

} // namespace System