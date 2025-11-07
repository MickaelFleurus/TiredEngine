#pragma once

#include "engine/system/DisplayParameter.h"
#include "engine/utils/FileHandler.h"

#include <string>

namespace System {
class CSystem {
public:
    CSystem();
    virtual ~CSystem() = default;

    bool Initialize();

    const std::string& GetGameName() const;
    Utils::CFileHandler& GetFileHandler();
    const SDisplayParameter& GetDisplayParameters() const;

private:
    std::string mGameName;
    Utils::CFileHandler mFileHandler;
    SDisplayParameter mDisplayParameters;
};
} // namespace System