#include "engine/system/System.h"

#include <format>

#include "engine/utils/Logger.h"

namespace {
constexpr const char* kSystemFile = "system_config";
}

namespace System {
CSystem::CSystem() : mAssetsParser(mFileHandler) {
}

CSystem::~CSystem() {
    Utils::Logger::Shutdown();
}

Utils::CFileHandler& CSystem::GetFileHandler() {
    return mFileHandler;
}

const SDisplayParameter& CSystem::GetDisplayParameters() const {
    return mDisplayParameters;
}

CAssetParser& CSystem::GetAssetParser() {
    return mAssetsParser;
}

const std::string& CSystem::GetGameName() const {
    return mGameName;
}

bool CSystem::Initialize() {
    auto json = mFileHandler.LoadJson(
        std::format("{}/{}.json", mFileHandler.GetAssetsFolder(), kSystemFile));
    if (json.is_null()) {
        return false;
    }
    if (json.contains("parameter")) {
        mGameName = json["parameter"].value("gameName", "DefaultGameName");
        mFileHandler.CreateTempFolder(mGameName);
        Utils::Logger::Init(mFileHandler.GetTempFolder() + "/logs/");

        if (json.contains("settings")) {
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
    }
    mAssetsParser.InitializeMonitoredFolders();

    return true;
}

} // namespace System
