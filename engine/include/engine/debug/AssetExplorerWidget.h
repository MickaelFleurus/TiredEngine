#pragma once

#include <filesystem>
#include <string>

#include "engine/debug/IOverlordItem.h"

namespace Utils {
class CFileHandler;
}

namespace Debug {
class CAssetExplorerWidget : public IOverlordItem {
public:
    explicit CAssetExplorerWidget(const Utils::CFileHandler& fileHandler);

    void Render() override;
    const char* GetName() const override;

private:
    void RenderDirectory(const std::filesystem::path& directory,
                         const std::filesystem::path& root);
    void RenderFiles(const std::filesystem::path& directory,
                     const std::filesystem::path& root);
    void RenderFileDetails(const std::filesystem::path& root);

    const Utils::CFileHandler& mFileHandler;
    std::filesystem::path mSelectedFolder;
    std::filesystem::path mSelectedAsset;
    std::string mFilter;
};
} // namespace Debug
