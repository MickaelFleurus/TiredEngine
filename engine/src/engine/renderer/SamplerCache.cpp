#include "engine/renderer/SamplerCache.h"

#include <SDL3/SDL_gpu.h>

namespace Renderer {
CSamplerCache::CSamplerCache(SDL_GPUDevice* device) : mDevice(device) {

    SDL_GPUSamplerCreateInfo linearDesc{};
    linearDesc.min_filter = SDL_GPU_FILTER_LINEAR;
    linearDesc.mag_filter = SDL_GPU_FILTER_LINEAR;
    linearDesc.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR;
    linearDesc.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    linearDesc.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    mLinear = SDL_CreateGPUSampler(mDevice, &linearDesc);

    SDL_GPUSamplerCreateInfo nearestDesc{};
    nearestDesc.min_filter = SDL_GPU_FILTER_NEAREST;
    nearestDesc.mag_filter = SDL_GPU_FILTER_NEAREST;
    nearestDesc.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    nearestDesc.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    nearestDesc.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    mNearest = SDL_CreateGPUSampler(mDevice, &nearestDesc);

    SDL_GPUSamplerCreateInfo repeatDesc{};
    repeatDesc.min_filter = SDL_GPU_FILTER_LINEAR;
    repeatDesc.mag_filter = SDL_GPU_FILTER_LINEAR;
    repeatDesc.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    repeatDesc.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT;
    mRepeat = SDL_CreateGPUSampler(mDevice, &repeatDesc);
}

CSamplerCache::~CSamplerCache() {
    if (mLinear) {
        SDL_ReleaseGPUSampler(mDevice, mLinear);
    }
    if (mNearest) {
        SDL_ReleaseGPUSampler(mDevice, mNearest);
    }
    if (mRepeat) {
        SDL_ReleaseGPUSampler(mDevice, mRepeat);
    }
}

SDL_GPUSampler* CSamplerCache::GetLinear() const {
    return mLinear;
}
SDL_GPUSampler* CSamplerCache::GetNearest() const {
    return mNearest;
}
SDL_GPUSampler* CSamplerCache::GetRepeat() const {
    return mRepeat;
}
} // namespace Renderer
