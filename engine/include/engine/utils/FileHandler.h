#pragma once

#include <filesystem>
#include <string>

#include <nlohmann/json.hpp>

struct SDL_Surface;

namespace Utils {

class CFileHandler {
public:
    enum class ETextureExtension { PNG, JPG };
    CFileHandler();

    void CreateTempFolder(const std::string& gameName);
    bool DoesFileExist(const std::string& filePath) const;

    std::string GetTempFolder() const;
    std::string GetAssetsFolder(std::string type = std::string()) const;
    std::filesystem::path
    ResolveSourceAssetPath(const std::filesystem::path& runtimePath) const;

    bool SaveTextureFile(const std::string& fileName, SDL_Surface* surface,
                         ETextureExtension ext = ETextureExtension::PNG) const;
    bool SaveJson(const std::string& fileName, const std::string& folderName,
                  const nlohmann::json& data) const;

    nlohmann::json LoadJson(const std::string& filePath) const;

    SDL_Surface* LoadTextureFile(const std::string& filePath) const;

private:
    std::filesystem::path mTempFolder;
    const std::filesystem::path mAssetFolder;
};
} // namespace Utils
