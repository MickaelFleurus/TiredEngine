#include "engine/utils/FileHandler.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_surface.h>
#include <filesystem>
#include <format>
#include <fstream>

namespace {
constexpr const char* kCompanyName = "PotatoThunder";
constexpr const char* kAssetsFolderPath = "assets/";
constexpr const char* kTexturesFolderPath = "assets/textures/";
constexpr const char* kFontsFolderPath = "assets/fonts/";

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

bool CFileHandler::SaveTextureFileBMP(const std::string& filename,
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

SDL_Surface* CFileHandler::LoadTextureFileBMP(const std::string& filename) {
    SDL_Surface* surface = SDL_LoadBMP(GetTexturePath(filename).c_str());
    return surface;
}

bool CFileHandler::DeleteJson(const std::string& filename) {
    auto completePath = GetJsonPath(filename);
    return std::filesystem::remove(completePath);
}

std::vector<std::string> CFileHandler::GetFonts() const {
    std::filesystem::path fontsPath = kFontsFolderPath;
    std::vector<std::string> fonts;
    for (auto const& dir_entry :
         std::filesystem::directory_iterator{fontsPath}) {
        if (dir_entry.path().extension() == ".ttf") {
            fonts.push_back(dir_entry.path().string());
        }
    }
    return fonts;
}

std::string CFileHandler::GetTexturePath(const std::string& filename,
                                         const std::string& ext) {
    return std::format("{}textures/{}.{}", mTempFolder, filename, ext);
}

std::string CFileHandler::GetJsonPath(const std::string& filename) {
    return std::format("{}textures/{}.json", mTempFolder, filename);
}

} // namespace Utils