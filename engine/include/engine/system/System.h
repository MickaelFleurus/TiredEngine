#pragma once

#include <string>

#include "engine/system/DisplayParameter.h"
#include "engine/utils/AssetParser.h"
#include "engine/utils/FileHandler.h"

namespace System {
class CSystem {
public:
    CSystem();
    ~CSystem();

    bool Initialize();

    const std::string& GetGameName() const;
    Utils::CFileHandler& GetFileHandler();
    const SDisplayParameter& GetDisplayParameters() const;
    CAssetParser& GetAssetParser();

private:
    std::string mGameName;
    Utils::CFileHandler mFileHandler;
    SDisplayParameter mDisplayParameters;
    CAssetParser mAssetsParser;
};
} // namespace System
