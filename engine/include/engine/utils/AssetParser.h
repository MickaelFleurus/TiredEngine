#pragma once
#include <optional>

#include "engine/utils/Asset.h"

namespace Utils {
class CFileHandler;
}

class CAssetParser {
public:
    explicit CAssetParser(Utils::CFileHandler& fileHandler);
    void InitializeMonitoredFolders();

    void CheckAndUpdateAssets();

    std::vector<SAsset> Get(EAssetType type) const;
    std::optional<std::reference_wrapper<const SAsset>>
    Get(EAssetType type, const std::string& name) const;

private:
    void Parse();
    Utils::CFileHandler& mFileHandler;
    std::unordered_map<SAssetKey, SAsset, SAssetKeyHash> mAssets;

    // Track folder modification times
    struct FolderState {
        std::filesystem::path path;
        std::filesystem::file_time_type lastWriteTime;
    };

    std::vector<FolderState> mMonitoredFolders;

    bool HasFoldersChanged() const;
};
