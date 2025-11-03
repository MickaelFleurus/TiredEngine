#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct SDL_Surface;

namespace Utils {

class CFileHandler {
public:
    CFileHandler(const std::string& gameName);

    std::string GetTempFolder() const;

    bool DoesFileExist(const std::string& filePath) const;
    bool DoesTextureExist(const std::string& filename) const;

    bool SaveTextureFileBMP(const std::string& filename, SDL_Surface* surface);
    bool SaveJson(const std::string& filePath, const nlohmann::json& data);

    ;
    nlohmann::json LoadJson(const std::string& filePath);
    SDL_Surface* LoadTextureFileBMP(const std::string& filename);

    std::vector<std::string> GetFonts() const;

    bool DeleteJson(const std::string& filePath);

private:
    std::string GetAbsolutePath(const std::string& relativePath) const;
    std::string GetTexturePath(const std::string& filename,
                               const std::string& ext = "bmp") const;
    std::string GetTextureMetaPath(const std::string& filename) const;

    std::string mTempFolder;
};
} // namespace Utils
