#pragma once

class SDL_GPUDevice;
class SDL_GPUSampler;

namespace Renderer {
class CSamplerCache {
public:
    CSamplerCache(SDL_GPUDevice& device);
    ~CSamplerCache();
    SDL_GPUSampler* GetLinear() const;
    SDL_GPUSampler* GetNearest() const;
    SDL_GPUSampler* GetRepeat() const;

private:
    SDL_GPUDevice& mDevice;
    SDL_GPUSampler* mLinear = nullptr;
    SDL_GPUSampler* mNearest = nullptr;
    SDL_GPUSampler* mRepeat = nullptr;
};
} // namespace Renderer