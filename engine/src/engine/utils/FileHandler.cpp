#include "engine/utils/FileHandler.h"

#include <filesystem>
#include <format>
#include <fstream>

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_surface.h>

namespace {
constexpr const char* kCompanyName = "PotatoThunder";
constexpr const char* kAssetsFolderPath = "assets/";

std::string HandleSDLPath(char* sdlPath) {
    std::string path;
    if (sdlPath) {
        path = std::string(sdlPath);
        SDL_free(sdlPath);
    }
    return path;
}

} // namespace

namespace Utils {

CFileHandler::CFileHandler()
    : mAssetFolder(std::format("{}{}", SDL_GetBasePath(), kAssetsFolderPath)) {
}

void CFileHandler::CreateTempFolder(const std::string& gameName) {
    const std::string tempPath =
        HandleSDLPath(SDL_GetPrefPath(kCompanyName, gameName.c_str()));
    mTempFolder = tempPath;
    CreateDirectories(mTempFolder);
}

std::string CFileHandler::GetTempFolder() const {
    return mTempFolder;
}

std::string CFileHandler::GetAssetsFolder() const {
    return mAssetFolder;
}

bool CFileHandler::DoesFileExist(const std::string& filePath,
                                 const char* extension) const {
    return std::filesystem::exists(filePath + extension);
}

bool CFileHandler::DoesDirectoryExists(const std::string& filePath) const {
    return std::filesystem::exists(filePath) &&
           std::filesystem::is_directory(filePath);
}

void CFileHandler::CreateDirectories(const std::string& path) const {
    std::filesystem::create_directories(path);
}

void CFileHandler::CreateDirectories(const std::filesystem::path& path) const {
    std::filesystem::create_directories(path);
}

bool CFileHandler::DeleteFile(const std::string& filePath,
                              const char* extension) {
    return std::filesystem::remove(filePath + extension);
}

bool CFileHandler::SaveTextureFileBMP(const std::string& filePath,
                                      SDL_Surface* surface) {
    if (const auto parent = std::filesystem::path(filePath).parent_path();
        !DoesDirectoryExists(parent.string())) {
        CreateDirectories(parent);
    }
    const auto completePath = filePath + ".bmp";
    if (SDL_SaveBMP(surface, completePath.c_str()) != 0) {
        return false;
    }
    return true;
}

bool CFileHandler::SaveJson(const std::string& filePath,
                            const nlohmann::json& data) {

    const auto completePath = filePath + ".json";
    std::ofstream fileStream(completePath, std::ios::out);
    if (!fileStream.is_open()) {
        return false;
    }

    fileStream << std::setw(4) << data << std::endl;
    fileStream.close();
    return true;
}

nlohmann::json CFileHandler::LoadJson(const std::string& filePath) {
    const auto completePath = filePath + ".json";
    std::ifstream fileStream(completePath, std::ios::in);
    if (!fileStream.is_open()) {
        return nlohmann::json{};
    }
    return nlohmann::json::parse(fileStream);
}

YAML::Node CFileHandler::LoadYAML(const std::string& filePath) {
    const auto completePath = filePath + ".yaml";
    return YAML::LoadFile(completePath);
}

SDL_Surface* CFileHandler::LoadTextureFileBMP(const std::string& filePath) {
    const auto completePath = filePath + ".bmp";
    SDL_Surface* surface = SDL_LoadBMP(completePath.c_str());
    return surface;
}

std::vector<std::string>
CFileHandler::GetFileNames(const char* extension) const {
    std::vector<std::string> files;
    for (auto const& dir_entry :
         std::filesystem::recursive_directory_iterator{mAssetFolder}) {
        if (dir_entry.path().extension() == extension) {
            files.push_back(dir_entry.path().stem().string());
        }
    }
    return files;
}

} // namespace Utils
