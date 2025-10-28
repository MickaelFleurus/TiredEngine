#include "engine/utils/FileHandler.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <format>
#include <fstream>

namespace {
constexpr const char* kCompanyName = "PotatoThunder";

} // namespace

namespace Utils {

CFileHandler::CFileHandler(const std::string& gameName)
    : mTempFolder(SDL_GetPrefPath(kCompanyName, gameName.c_str())) {
    std::filesystem::create_directories(std::format("{}textures", mTempFolder));
}

std::string CFileHandler::GetTempFolder() const {
    return mTempFolder;
}

bool CFileHandler::DoesTextureExist(const std::string& filename) {
    return std::filesystem::exists(GetTexturePath(filename));
}

bool CFileHandler::SaveTexture(const std::string& filename,
                               SDL_Surface* surface) {
    if (SDL_SaveBMP(surface, GetTexturePath(filename).c_str()) != 0) {
        return false;
    }
    return true;
}

bool CFileHandler::SaveJson(const std::string& filename,
                            const nlohmann::json& data) {

    auto completePath = GetJsonPath(filename);
    std::ofstream fileStream(completePath, std::ios::out);
    if (!fileStream.is_open()) {
        return false;
    }

    fileStream << std::setw(4) << data << std::endl;
    fileStream.close();
    return true;
}

nlohmann::json CFileHandler::LoadJson(const std::string& filename) {
    auto completePath = GetJsonPath(filename);
    std::ifstream fileStream(completePath);
    if (!fileStream.is_open()) {
        return nlohmann::json{};
    }
    return nlohmann::json::parse(fileStream);
}

SDL_Surface* CFileHandler::LoadTexture(const std::string& filename) {
    SDL_Surface* surface = SDL_LoadBMP(GetTexturePath(filename).c_str());
    return surface;
}

bool CFileHandler::DeleteJson(const std::string& filename) {
    auto completePath = GetJsonPath(filename);
    return std::filesystem::remove(completePath);
}

std::string CFileHandler::GetTexturePath(const std::string& filename) {
    return std::format("{}textures/{}.bmp", mTempFolder, filename);
}

std::string CFileHandler::GetJsonPath(const std::string& filename) {
    return std::format("{}textures/{}.json", mTempFolder, filename);
}

} // namespace Utils