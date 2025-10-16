#include "renderer/ShaderFactory.h"
#include "renderer/Shader.h"

#include "SDL3/SDL_filesystem.h"
#include "SDL3/SDL_gpu.h"
#include "SDL3/SDL_iostream.h"

#include <memory>
#include <optional>
#include <string>

namespace Renderer {

class CShaderFactory::CImpl {
public:
    CImpl(SDL_GPUDevice& device)
        : m_Device(device)

        , m_BasePath(SDL_GetBasePath()) {
    }

    CShader* CreateFragmentShader(const char* filePath) {
        return new CShader(CreateShader(filePath, SDL_GPU_SHADERSTAGE_FRAGMENT),
                           m_Device);
    }

    CShader* CreateVertexShader(const char* filePath) {
        return new CShader(CreateShader(filePath, SDL_GPU_SHADERSTAGE_VERTEX),
                           m_Device);
    }

private:
    SDL_GPUShader* CreateShader(const char* filePath,
                                SDL_GPUShaderStage stage) {
        // Determine the backend format and construct file path
        SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(&m_Device);

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
            std::string(m_BasePath) + filePath + config->extension;

        // Load shader file
        size_t codeSize = 0;
        void* rawCode = SDL_LoadFile(fullPath.c_str(), &codeSize);
        if (rawCode == nullptr) {
            return nullptr;
        }

        // Use RAII for automatic cleanup
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

        SDL_GPUShader* shader = SDL_CreateGPUShader(&m_Device, &shaderInfo);

        return shader; // code automatically freed by unique_ptr
    }

    const char* m_BasePath;
    SDL_GPUDevice& m_Device;
};

CShaderFactory::CShaderFactory(SDL_GPUDevice& device)
    : m_Impl(std::make_unique<CImpl>(device)) {
}

CShaderFactory::~CShaderFactory() = default;

CShader* CShaderFactory::CreateFragmentShader(const char* filePath) {
    return m_Impl->CreateFragmentShader(filePath);
}

CShader* CShaderFactory::CreateVertexShader(const char* filePath) {
    return m_Impl->CreateVertexShader(filePath);
}

} // namespace Renderer