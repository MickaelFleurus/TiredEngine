#include "engine/utils/FileHandler.h"

#include <filesystem>
#include <format>
#include <fstream>
#include <string>

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
    mTempFolder =
        HandleSDLPath(SDL_GetPrefPath(kCompanyName, gameName.c_str()));
    std::filesystem::create_directories(mTempFolder);
    std::filesystem::create_directories(mTempFolder / "textures/");
    std::filesystem::create_directories(mTempFolder / "logs/");
}

std::string CFileHandler::GetTempFolder() const {
    return mTempFolder;
}

std::string CFileHandler::GetAssetsFolder(std::string type) const {
    return (mAssetFolder / type).string();
}

std::filesystem::path CFileHandler::ResolveSourceAssetPath(
    const std::filesystem::path& runtimePath) const {
    std::error_code error;
    const auto relativePath =
        std::filesystem::relative(runtimePath, mAssetFolder, error);
    if (error || relativePath.empty() || relativePath.is_absolute() ||
        *relativePath.begin() == "..") {
        return runtimePath;
    }

    const auto sourceMap = mAssetFolder / ".asset_sources";
    std::ifstream input(sourceMap);
    std::string line;
    const auto requested = relativePath.generic_string();
    std::filesystem::path resolved = runtimePath;
    while (std::getline(input, line)) {
        const auto separator = line.find('|');
        if (separator == std::string::npos ||
            line.substr(0, separator) != requested) {
            continue;
        }

        const std::filesystem::path candidate{line.substr(separator + 1)};
        if (std::filesystem::exists(candidate)) {
            resolved = candidate;
        }
    }
    return resolved;
}

bool CFileHandler::DoesFileExist(const std::string& filePath) const {
    return std::filesystem::exists(filePath);
}

bool CFileHandler::SaveTextureFile(const std::string& fileName,
                                   SDL_Surface* surface,
                                   ETextureExtension ext) const {

    auto completePath = mTempFolder / "textures" / fileName;
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

    std::filesystem::path completePath = mTempFolder / folderName;

    std::filesystem::create_directories(completePath);
    completePath.append(fileName + ".json");
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

SDL_Surface* CFileHandler::LoadTextureFile(const std::string& filePath) const {
    return IMG_Load(filePath.c_str());
}

} // namespace Utils
