#pragma once

#include <memory>
#include <string>
struct SDL_GPUDevice;
struct SDL_GPUShader;

namespace Renderer {

struct SShaderResources {
    uint32_t numSamplers = 0;
    uint32_t numStorageTextures = 0;
    uint32_t numStorageBuffers = 0;
    uint32_t numUniformBuffers = 0;
};

class CShaderFactory {
public:
    CShaderFactory(SDL_GPUDevice& device);
    ~CShaderFactory();

    SDL_GPUShader* CreateFragmentShader(std::string name, std::string path,
                                        const SShaderResources& resources = {});
    SDL_GPUShader* CreateVertexShader(std::string name, std::string path,
                                      const SShaderResources& resources = {});

private:
    class CImpl;
    std::unique_ptr<CImpl> mImpl;
};
} // namespace Renderer