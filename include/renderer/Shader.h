#pragma once
#include <memory>
class SDL_GPUShader;
class SDL_GPUDevice;

namespace Renderer {
class CShader {
public:
    CShader(SDL_GPUShader* shader, SDL_GPUDevice& device);
    ~CShader();
    SDL_GPUShader* Get() const;

private:
    SDL_GPUDevice& m_Device;
    SDL_GPUShader* m_Shader;
};
} // namespace Renderer