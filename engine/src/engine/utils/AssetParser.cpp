#include "engine/utils/AssetParser.h"

#include <array>
#include <filesystem>

#include "engine/utils/FileHandler.h"
#include "engine/utils/StringUtils.h"

namespace fs = std::filesystem;
namespace {
constexpr std::string_view kTexturePNGExtension{".png"};
constexpr std::string_view kTextureJPGExtension{".jpg"};
constexpr std::string_view kFontExtension{".ttf"};
constexpr std::string_view kVertexShaderExtension{".vert"};
constexpr std::string_view kFragmentShaderExtension{".frag"};

constexpr std::string_view kMetadataJson{".json"};
constexpr std::string_view kMetadataYaml{".yaml"};
constexpr std::array<std::string_view, 2> kMetadataExtensions{kMetadataJson,
                                                              kMetadataYaml};

std::optional<fs::path> SearchMetaFile(fs::path filePath) {
    for (const auto& ext : kMetadataExtensions) {
        filePath.replace_extension(ext);
        if (fs::exists(filePath)) {
            return filePath;
        }
    }
    return std::nullopt;
}

constexpr std::array<std::string_view, 5> kAssetExtensions{
    kTexturePNGExtension, kTextureJPGExtension, kFontExtension,
    kVertexShaderExtension, kFragmentShaderExtension};

constexpr EAssetType ExtToType(const char* ext) {
    if (ext == kTexturePNGExtension) {
        return EAssetType::Texture;
    } else if (ext == kTextureJPGExtension) {
        return EAssetType::Texture;
    } else if (ext == kFontExtension) {
        return EAssetType::Font;
    } else if (ext == kVertexShaderExtension) {
        return EAssetType::Shader;
    } else if (ext == kFragmentShaderExtension) {
        return EAssetType::Shader;
    }
    return EAssetType::Unknown;
};

} // namespace

CAssetParser::CAssetParser(Utils::CFileHandler& fileHandler)
    : mFileHandler(fileHandler) {
}

void CAssetParser::Parse() {
    for (auto& folder : mMonitoredFolders) {
        for (const auto& entry :
             fs::recursive_directory_iterator(folder.path)) {
            if (fs::is_directory(entry)) {
                continue;
            }
            const auto path = entry.path();
            const auto extension = path.extension();
            const auto type = ExtToType(extension.c_str());
            const SAssetKey key{Utils::ToLowercase(path.stem().string()), type};
            if (type == EAssetType::Unknown || mAssets.contains(key)) {
                continue;
            }
            mAssets.emplace(key, SAsset{
                                     .mType = type,
                                     .mMetadata = SearchMetaFile(path),
                                     .mPath = path,
                                 });
        }
        folder.lastWriteTime = fs::last_write_time(folder.path);
    }
}

void CAssetParser::InitializeMonitoredFolders() {
    mMonitoredFolders.clear();
    const auto paths = {mFileHandler.GetAssetsFolder(),
                        mFileHandler.GetTempFolder()};
    for (const auto& path : paths) {
        if (fs::exists(path)) {
            mMonitoredFolders.push_back({path, fs::last_write_time(path)});
        }
    }
    Parse();
}

bool CAssetParser::HasFoldersChanged() const {
    for (const auto& folder : mMonitoredFolders) {
        if (!fs::exists(folder.path)) {
            return true; // Folder deleted
        }
        if (fs::last_write_time(folder.path) != folder.lastWriteTime) {
            return true; // Folder modified
        }
    }
    return false;
}

void CAssetParser::CheckAndUpdateAssets() {
    if (HasFoldersChanged()) {
        mAssets.clear();
        Parse(); // Re-parse everything
    }
}

std::vector<SAsset> CAssetParser::Get(EAssetType type) const {
    std::vector<SAsset> assets;
    for (const auto& asset : mAssets) {
        if (asset.first.type == type) {
            assets.push_back(asset.second);
        }
    }
    return std::move(assets);
}

std::optional<std::reference_wrapper<const SAsset>>
CAssetParser::Get(EAssetType type, const std::string& name) const {
    const SAssetKey key{Utils::ToLowercase(name), type};
    if (!mAssets.contains(key)) {
        return std::nullopt;
    }
    return mAssets.at(key);
}
