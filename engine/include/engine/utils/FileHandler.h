#pragma once
#include <nlohmann/json.hpp>
#include <string>

struct SDL_Surface;

namespace Utils {

class CFileHandler {
public:
    CFileHandler(const std::string& gameName);

    std::string GetTempFolder() const;

    bool DoesTextureExist(const std::string& filename);

    bool SaveTexture(const std::string& filename, SDL_Surface* surface);
    bool SaveJson(const std::string& filename, const nlohmann::json& data);

    nlohmann::json LoadJson(const std::string& filename);
    SDL_Surface* LoadTexture(const std::string& filename);

    bool DeleteJson(const std::string& filename);

private:
    std::string GetTexturePath(const std::string& filename);
    std::string GetJsonPath(const std::string& filename);

    std::string mTempFolder;
};
} // namespace Utils
