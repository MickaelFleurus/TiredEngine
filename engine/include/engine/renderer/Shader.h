#pragma once
#include <memory>
struct SDL_GPUShader;
struct SDL_GPUDevice;

namespace Renderer {
class CShader {
public:
    CShader(SDL_GPUShader* shader, SDL_GPUDevice& device);
    ~CShader();
    SDL_GPUShader* Get() const;

private:
    SDL_GPUDevice& mDevice;
    SDL_GPUShader* mShader;
};
} // namespace Renderer