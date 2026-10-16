#pragma once
#include <filesystem>
#include <optional>
#include <string>

#include "engine/utils/Hashing.h"

enum class EAssetType { Texture, Shader, Level, Font, Unknown };
struct SAsset {
    EAssetType mType;
    std::optional<std::filesystem::path> mMetadata{std::nullopt};
    std::filesystem::path mPath;
};

struct SAssetKey {
    std::string id;
    EAssetType type;

    bool operator==(const SAssetKey& other) const {
        return id == other.id && type == other.type;
    }
};

struct SAssetKeyHash {
    size_t operator()(const SAssetKey& k) const {
        return Utils::CreateHash(k.id, k.type);
    }
};
