#include "engine/renderer/TextureManager.h"
#include "engine/utils/FileHandler.h"

#include <SDL3/SDL_gpu.h>

namespace Renderer {
CTextureManager::CTextureManager(SDL_GPUDevice* device,
                                 Utils::CFileHandler& fileHandler)
    : mDevice(device), mFileHandler(fileHandler) {
}

CTextureManager::~CTextureManager() {
    for (auto& pair : mLoadedTextures) {
        SDL_ReleaseGPUTexture(mDevice, pair.second);
    }
}

SDL_GPUTexture* CTextureManager::LoadTexture(const std::string& filename) {
    auto it = mLoadedTextures.find(filename);
    if (it != mLoadedTextures.end()) {
        return it->second;
    }

    SDL_Surface* surface = mFileHandler.LoadTextureFileBMP(filename);
    if (!surface) {
        return nullptr;
    }
    auto texture = LoadTextureFromSurface(filename, surface);

    SDL_DestroySurface(surface);
    return texture;
}

SDL_GPUTexture*
CTextureManager::LoadTextureFromSurface(const std::string& filename,
                                        SDL_Surface* surface) {

    SDL_GPUTextureCreateInfo texInfo = {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(surface->w),
        .height = static_cast<Uint32>(surface->h),
        .layer_count_or_depth = 1,
        .num_levels = 1};
    auto texture = SDL_CreateGPUTexture(mDevice, &texInfo);

    unsigned int uploadSize = surface->pitch * surface->h;
    SDL_GPUTransferBufferCreateInfo bufInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = uploadSize};
    SDL_GPUTransferBuffer* transferBuffer =
        SDL_CreateGPUTransferBuffer(mDevice, &bufInfo);

    void* ptr = SDL_MapGPUTransferBuffer(mDevice, transferBuffer, false);
    SDL_memcpy(ptr, surface->pixels, uploadSize);
    SDL_UnmapGPUTransferBuffer(mDevice, transferBuffer);

    SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(mDevice);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTextureTransferInfo src = {
        .transfer_buffer = transferBuffer,
        .offset = 0,
        .pixels_per_row = static_cast<Uint32>(surface->w),
        .rows_per_layer = static_cast<Uint32>(surface->h)};

    SDL_GPUTextureRegion dst = {.texture = texture,
                                .x = 0,
                                .y = 0,
                                .z = 0,
                                .w = static_cast<Uint32>(surface->w),
                                .h = static_cast<Uint32>(surface->h),
                                .d = 1};

    SDL_UploadToGPUTexture(copyPass, &src, &dst, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);

    SDL_ReleaseGPUTransferBuffer(mDevice, transferBuffer);

    if (texture) {
        mLoadedTextures[filename] = texture;
    }
    return texture;
}

SDL_GPUTexture* CTextureManager::GetTexture(const std::string& filename) {
    auto it = mLoadedTextures.find(filename);
    if (it != mLoadedTextures.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace Renderer