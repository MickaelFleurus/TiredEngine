#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct SDL_Surface;

namespace Utils {

class CFileHandler {
public:
    CFileHandler(const std::string& gameName);

    std::string GetTempFolder() const;
    std::string GetAssetsFolder() const;

    bool DoesFileExist(const std::string& filePath,
                       const char* extension) const;
    bool DoesDirectoryExists(const std::string& filePath) const;
    void CreateDirectories(const std::string& path) const;

    bool DeleteFile(const std::string& filePath, const char* extension);

    bool SaveTextureFileBMP(const std::string& filePath, SDL_Surface* surface);
    bool SaveJson(const std::string& filePath, const nlohmann::json& data);

    nlohmann::json LoadJson(const std::string& filePath);
    SDL_Surface* LoadTextureFileBMP(const std::string& filePath);

    std::vector<std::string> GetFileNames(const char* extension) const;

private:
    const std::string mTempFolder;
    const std::string mAssetFolder;
};
} // namespace Utils
