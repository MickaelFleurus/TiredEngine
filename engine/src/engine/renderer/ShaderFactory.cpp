#include "engine/renderer/ShaderFactory.h"
#include "engine/renderer/Shader.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>

#include <memory>
#include <optional>
#include <string>

namespace Renderer {

class CShaderFactory::CImpl {
public:
    CImpl(SDL_GPUDevice& device)
        : mDevice(device), mBasePath(SDL_GetBasePath()) {
    }

    CShader* CreateFragmentShader(const char* filePath) {
        return new CShader(CreateShader(filePath, SDL_GPU_SHADERSTAGE_FRAGMENT),
                           mDevice);
    }

    CShader* CreateVertexShader(const char* filePath) {
        return new CShader(CreateShader(filePath, SDL_GPU_SHADERSTAGE_VERTEX),
                           mDevice);
    }

private:
    SDL_GPUShader* CreateShader(const char* filePath,
                                SDL_GPUShaderStage stage) {
        // Determine the backend format and construct file path
        SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(&mDevice);

        struct ShaderConfig {
            SDL_GPUShaderFormat format;
            const char* extension;
            const char* entrypoint;
        };

        std::optional<ShaderConfig> config;

        if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
            config = ShaderConfig{SDL_GPU_SHADERFORMAT_SPIRV, ".spv", "main"};
        } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
            config = ShaderConfig{SDL_GPU_SHADERFORMAT_MSL, ".msl", "main0"};
        } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
            config = ShaderConfig{SDL_GPU_SHADERFORMAT_DXIL, ".dxil", "main"};
        } else {
            // Unrecognized backend shader format
            return nullptr;
        }

        // Build full path
        std::string fullPath =
            std::string(mBasePath) + filePath + config->extension;

        // Load shader file
        size_t codeSize = 0;
        void* rawCode = SDL_LoadFile(fullPath.c_str(), &codeSize);
        if (rawCode == nullptr) {
            return nullptr;
        }

        std::unique_ptr<void, decltype(&SDL_free)> code(rawCode, SDL_free);

        // Create shader info
        SDL_GPUShaderCreateInfo shaderInfo{
            .code_size = codeSize,
            .code = static_cast<const Uint8*>(code.get()),
            .entrypoint = config->entrypoint,
            .format = config->format,
            .stage = stage,
            .num_samplers = 0,
            .num_storage_textures = 0,
            .num_storage_buffers = 0,
            .num_uniform_buffers = 0};

        SDL_GPUShader* shader = SDL_CreateGPUShader(&mDevice, &shaderInfo);

        return shader; // code automatically freed by unique_ptr
    }

    const char* mBasePath;
    SDL_GPUDevice& mDevice;
};

CShaderFactory::CShaderFactory(SDL_GPUDevice& device)
    : mImpl(std::make_unique<CImpl>(device)) {
}

CShaderFactory::~CShaderFactory() = default;

CShader* CShaderFactory::CreateFragmentShader(const char* filePath) {
    return mImpl->CreateFragmentShader(filePath);
}

CShader* CShaderFactory::CreateVertexShader(const char* filePath) {
    return mImpl->CreateVertexShader(filePath);
}

} // namespace Renderer