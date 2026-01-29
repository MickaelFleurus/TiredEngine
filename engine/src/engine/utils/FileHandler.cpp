#include "engine/utils/FileHandler.h"

#include <filesystem>
#include <format>
#include <fstream>

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>

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
    std::filesystem::create_directories(mTempFolder);
    std::filesystem::create_directories(mTempFolder + "/textures/");
    std::filesystem::create_directories(mTempFolder + "/logs/");
}

std::string CFileHandler::GetTempFolder() const {
    return mTempFolder;
}

std::string CFileHandler::GetAssetsFolder() const {
    return mAssetFolder;
}

bool CFileHandler::DoesFileExist(const std::string& filePath) const {
    return std::filesystem::exists(filePath);
}

bool CFileHandler::SaveTextureFile(const std::string& fileName,
                                   SDL_Surface* surface,
                                   ETextureExtension ext) const {

    auto completePath = mTempFolder + "/textures/" + fileName;
    if (ext == ETextureExtension::JPG) {
        completePath += ".jpg";
        return IMG_SaveJPG(surface, completePath.c_str(), 100);
    } else {
        completePath += ".png";
        return IMG_SavePNG(surface, completePath.c_str());
    }
}

bool CFileHandler::SaveJson(const std::string& fileName,
                            const std::string& folderName,
                            const nlohmann::json& data) const {

    const std::filesystem::path completePath =
        mTempFolder + "/" + folderName + "/" + fileName + ".json";
    std::filesystem::create_directories(completePath.parent_path());

    std::ofstream fileStream(completePath.c_str(), std::ios::out);
    if (!fileStream.is_open()) {
        return false;
    }

    fileStream << std::setw(4) << data << std::endl;
    fileStream.close();
    return true;
}

nlohmann::json CFileHandler::LoadJson(const std::string& filePath) const {
    std::ifstream fileStream(filePath, std::ios::in);
    if (!fileStream.is_open()) {
        return nlohmann::json{};
    }
    return nlohmann::json::parse(fileStream);
}

YAML::Node CFileHandler::LoadYAML(const std::string& filePath) const {
    return YAML::LoadFile(filePath);
}

SDL_Surface* CFileHandler::LoadTextureFile(const std::string& filePath) const {
    return IMG_Load(filePath.c_str());
}

} // namespace Utils
