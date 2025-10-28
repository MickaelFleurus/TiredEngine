#pragma once
#include <string>
#include <unordered_map>

struct SDL_GPUDevice;
struct SDL_GPUTexture;
struct SDL_Surface;
namespace Utils {
class CFileHandler;
}

namespace Renderer {
class CTextureManager {
public:
    CTextureManager(SDL_GPUDevice* device, Utils::CFileHandler& fileHandler);
    ~CTextureManager();

    SDL_GPUTexture* LoadTexture(const std::string& filename);
    SDL_GPUTexture* LoadTextureFromSurface(const std::string& filename,
                                           SDL_Surface* surface);
    SDL_GPUTexture* GetTexture(const std::string& filename);

private:
    SDL_GPUDevice* mDevice{nullptr};
    Utils::CFileHandler& mFileHandler;

    std::unordered_map<std::string, SDL_GPUTexture*> mLoadedTextures;
};
} // namespace Renderer