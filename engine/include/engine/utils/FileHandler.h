#pragma once

#include <string>

#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

struct SDL_Surface;

namespace Utils {

class CFileHandler {
public:
    CFileHandler();

    void CreateTempFolder(const std::string& gameName);

    std::string GetTempFolder() const;
    std::string GetAssetsFolder() const;

    bool DoesFileExist(const std::string& filePath,
                       const char* extension) const;
    bool DoesDirectoryExists(const std::string& filePath) const;
    void CreateDirectories(const std::string& path) const;
    void CreateDirectories(const std::filesystem::path& path) const;

    bool DeleteFile(const std::string& filePath, const char* extension);

    bool SaveTextureFileBMP(const std::string& filePath, SDL_Surface* surface);
    bool SaveJson(const std::string& filePath, const nlohmann::json& data);

    nlohmann::json LoadJson(const std::string& filePath);
    YAML::Node LoadYAML(const std::string& filePath);
    SDL_Surface* LoadTextureFileBMP(const std::string& filePath);

    std::vector<std::string> GetFileNames(const char* extension) const;

private:
    std::string mTempFolder;
    const std::string mAssetFolder;
};
} // namespace Utils
