#include "engine/renderer/ShaderFactory.h"

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_iostream.h>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

namespace {
constexpr const char* ShaderStageToExtension(SDL_GPUShaderStage stage) {
    switch (stage) {
    case SDL_GPU_SHADERSTAGE_VERTEX:
        return ".vert";
    case SDL_GPU_SHADERSTAGE_FRAGMENT:
        return ".frag";
    default:
        return "";
    }
}
} // namespace

namespace Renderer {

class CShaderFactory::CImpl {
public:
    CImpl(SDL_GPUDevice& device) : mDevice(device) {
    }

    ~CImpl() {
        for (auto& [path, shader] : mShaderCache) {
            SDL_ReleaseGPUShader(&mDevice, shader);
        }
    }

    SDL_GPUShader* CreateFragmentShader(std::string name, std::string path,
                                        const SShaderResources& resources) {
        return CreateShader(name, path, SDL_GPU_SHADERSTAGE_FRAGMENT,
                            resources);
    }

    SDL_GPUShader* CreateVertexShader(std::string name, std::string path,
                                      const SShaderResources& resources) {
        return CreateShader(name, path, SDL_GPU_SHADERSTAGE_VERTEX, resources);
    }

private:
    SDL_GPUShader* CreateShader(std::string name, std::string path,
                                SDL_GPUShaderStage stage,
                                const SShaderResources& resources) {
        name += ShaderStageToExtension(stage);
        path += name;
        if (!mShaderCache.contains(name)) {

            SDL_GPUShaderFormat backendFormats =
                SDL_GetGPUShaderFormats(&mDevice);

            struct ShaderConfig {
                SDL_GPUShaderFormat format;
                const char* extension;
                const char* entrypoint;
            };

            std::optional<ShaderConfig> config;

            if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
                config =
                    ShaderConfig{SDL_GPU_SHADERFORMAT_SPIRV, ".spv", "main"};
            } else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
                config =
                    ShaderConfig{SDL_GPU_SHADERFORMAT_MSL, ".msl", "main0"};
            } else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
                config =
                    ShaderConfig{SDL_GPU_SHADERFORMAT_DXIL, ".dxil", "main"};
            } else {
                // Unrecognized backend shader format
                return nullptr;
            }

            std::string fullPath = path + config->extension;
            size_t codeSize = 0;
            void* rawCode = SDL_LoadFile(fullPath.c_str(), &codeSize);
            if (rawCode == nullptr) {
                return nullptr;
            }

            std::unique_ptr<void, decltype(&SDL_free)> code(rawCode, SDL_free);

            SDL_GPUShaderCreateInfo shaderInfo{
                .code_size = codeSize,
                .code = static_cast<const Uint8*>(code.get()),
                .entrypoint = config->entrypoint,
                .format = config->format,
                .stage = stage,
                .num_samplers = resources.numSamplers,
                .num_storage_textures = resources.numStorageTextures,
                .num_storage_buffers = resources.numStorageBuffers,
                .num_uniform_buffers = resources.numUniformBuffers};

            SDL_GPUShader* shader = SDL_CreateGPUShader(&mDevice, &shaderInfo);
            mShaderCache.emplace(name, shader);
        }
        return mShaderCache[name];
    }

    SDL_GPUDevice& mDevice;
    std::unordered_map<std::string, SDL_GPUShader*> mShaderCache;
};

CShaderFactory::CShaderFactory(SDL_GPUDevice& device)
    : mImpl(std::make_unique<CImpl>(device)) {
}

CShaderFactory::~CShaderFactory() = default;

SDL_GPUShader*
CShaderFactory::CreateFragmentShader(std::string name, std::string path,
                                     const SShaderResources& resources) {
    return mImpl->CreateFragmentShader(name, path, resources);
}

SDL_GPUShader*
CShaderFactory::CreateVertexShader(std::string name, std::string path,
                                   const SShaderResources& resources) {
    return mImpl->CreateVertexShader(name, path, resources);
}

} // namespace Renderer