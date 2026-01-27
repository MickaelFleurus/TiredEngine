#pragma once

#include <string>

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

struct SDL_Surface;

namespace Utils {

class CFileHandler {
public:
    enum class ETextureExtension { PNG, JPG };
    CFileHandler();

    void CreateTempFolder(const std::string& gameName);
    bool DoesFileExist(const std::string& filePath,
                       const char* extension) const;

    std::string GetTempFolder() const;
    std::string GetAssetsFolder() const;

    bool SaveTextureFile(const std::string& fileName, SDL_Surface* surface,
                         ETextureExtension ext = ETextureExtension::PNG) const;
    bool SaveJson(const std::string& fileName, const std::string& folderName,
                  const nlohmann::json& data) const;

    nlohmann::json LoadJson(const std::string& filePath) const;
    YAML::Node LoadYAML(const std::string& filePath) const;
    SDL_Surface* LoadTextureFile(const std::string& filePath) const;

private:
    std::string mTempFolder;
    const std::string mAssetFolder;
};
} // namespace Utils
