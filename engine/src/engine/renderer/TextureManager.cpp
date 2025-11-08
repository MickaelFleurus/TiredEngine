#include "engine/renderer/TextureManager.h"
#include "engine/renderer/Window.h"
#include "engine/utils/FileHandler.h"

#include <SDL3/SDL_gpu.h>

namespace {
constexpr auto kGPUTransferBufferDeleter = [](SDL_GPUDevice* device) {
    return [device](SDL_GPUTransferBuffer* buffer) {
        if (buffer) {
            SDL_ReleaseGPUTransferBuffer(device, buffer);
        }
    };
};

constexpr auto kGPUTextureDeleter = [](SDL_GPUDevice* device) {
    return [device](SDL_GPUTexture* texture) {
        if (texture) {
            SDL_ReleaseGPUTexture(device, texture);
        }
    };
};

constexpr auto kGPUSurfaceDeleter = [](SDL_Surface* surface) {
    if (surface) {
        SDL_DestroySurface(surface);
    }
};

} // namespace

namespace Renderer {
CTextureManager::CTextureManager(const CWindow& window,
                                 Utils::CFileHandler& fileHandler)
    : mWindow(window), mFileHandler(fileHandler) {
}

CTextureManager::~CTextureManager() {
    for (auto& pair : mLoadedTextures) {
        SDL_ReleaseGPUTexture(mWindow.GetDevice(), pair.second);
    }
}

SDL_GPUTexture* CTextureManager::LoadTexture(const std::string& filename) {
    auto it = mLoadedTextures.find(filename);
    if (it != mLoadedTextures.end()) {
        return it->second;
    }

    std::unique_ptr<SDL_Surface, decltype(kGPUSurfaceDeleter)> surface{
        mFileHandler.LoadTextureFileBMP(filename), kGPUSurfaceDeleter};
    if (!surface) {
        return nullptr;
    }
    auto texture = LoadTextureFromSurface(filename, surface.get());

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
    std::unique_ptr<SDL_GPUTexture,
                    decltype(kGPUTextureDeleter(mWindow.GetDevice()))>
        texture{SDL_CreateGPUTexture(mWindow.GetDevice(), &texInfo),
                kGPUTextureDeleter(mWindow.GetDevice())};

    unsigned int uploadSize = surface->pitch * surface->h;
    SDL_GPUTransferBufferCreateInfo bufInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD, .size = uploadSize};
    std::unique_ptr<SDL_GPUTransferBuffer,
                    decltype(kGPUTransferBufferDeleter(mWindow.GetDevice()))>
        transferBuffer{
            SDL_CreateGPUTransferBuffer(mWindow.GetDevice(), &bufInfo),
            kGPUTransferBufferDeleter(mWindow.GetDevice())};

    if (!transferBuffer) {
        return nullptr;
    }

    void* ptr = SDL_MapGPUTransferBuffer(mWindow.GetDevice(),
                                         transferBuffer.get(), false);
    if (!ptr) {
        return nullptr;
    }

    SDL_memcpy(ptr, surface->pixels, uploadSize);
    SDL_UnmapGPUTransferBuffer(mWindow.GetDevice(), transferBuffer.get());

    SDL_GPUCommandBuffer* cmd =
        SDL_AcquireGPUCommandBuffer(mWindow.GetDevice());

    if (!cmd) {
        return nullptr;
    }

    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(cmd);

    SDL_GPUTextureTransferInfo src = {
        .transfer_buffer = transferBuffer.get(),
        .offset = 0,
        .pixels_per_row = static_cast<Uint32>(surface->w),
        .rows_per_layer = static_cast<Uint32>(surface->h)};

    SDL_GPUTextureRegion dst = {.texture = texture.get(),
                                .x = 0,
                                .y = 0,
                                .z = 0,
                                .w = static_cast<Uint32>(surface->w),
                                .h = static_cast<Uint32>(surface->h),
                                .d = 1};

    SDL_UploadToGPUTexture(copyPass, &src, &dst, false);

    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(cmd);

    mLoadedTextures.emplace(filename, texture.release());
    return mLoadedTextures[filename];
}

SDL_GPUTexture* CTextureManager::GetTexture(const std::string& filename) {
    auto it = mLoadedTextures.find(filename);
    if (it != mLoadedTextures.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace Renderer