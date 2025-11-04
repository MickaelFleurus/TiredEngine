#pragma once

#include "engine/material/AbstractMaterial.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/Vertex.h"
#include <SDL3/SDL_gpu.h>
#include <concepts>
#include <cstdint>
#include <glm/mat4x4.hpp>
#include <ranges>
#include <span>

struct SDL_GPUBuffer;

namespace Material {
class AbstractMaterial;
}

namespace Renderer {

enum class ERenderLayer : uint8_t {
    Background = 0,
    World = 1,
    UI = 2,
    Debug = 3
};

struct SRenderable {
    glm::mat4 transform;
    Material::AbstractMaterial* material;
    ERenderLayer layer;
    float depth;

    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indexBuffer;
    uint32_t indexCount;

    SDL_GPUBuffer* instanceBuffer = nullptr;
    uint32_t instanceCount = 0;

    uint64_t sortKey;

    void GenerateSortKey() {
        sortKey = (static_cast<uint64_t>(layer) << 56) |
                  (reinterpret_cast<uint64_t>(material->GetPipeline()) & 0xFFFF)
                      << 40 |
                  (reinterpret_cast<uint64_t>(material->GetTexture()) & 0xFFFF)
                      << 24 |
                  (static_cast<uint64_t>(depth * 1000.0f) & 0xFFFFFF);
    }

    bool operator<(const SRenderable& other) const {
        return sortKey < other.sortKey;
    }
};

struct VertexLayoutInfo {
    std::vector<SDL_GPUVertexAttribute> attributes;
    std::vector<SDL_GPUVertexBufferDescription> bufferDescriptions;
};

template <typename T>
concept ContiguousRange = std::ranges::range<T> &&
                          std::contiguous_iterator<std::ranges::iterator_t<T>>;

template <ContiguousRange R>
SDL_GPUBuffer* CreateBuffer(SDL_GPUDevice& device, R& content,
                            SDL_GPUBufferUsageFlags bufferType) {
    uint32_t containerMemSize = static_cast<uint32_t>(
        sizeof(std::ranges::range_value_t<R>) * std::ranges::size(content));
    SDL_GPUBufferCreateInfo vertexInfo = {};
    vertexInfo.usage = bufferType;
    vertexInfo.size = containerMemSize;

    SDL_GPUBuffer* buffer = SDL_CreateGPUBuffer(&device, &vertexInfo);

    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = containerMemSize;

    SDL_GPUTransferBuffer* transferBuffer =
        SDL_CreateGPUTransferBuffer(&device, &transferInfo);

    void* data = SDL_MapGPUTransferBuffer(&device, transferBuffer, false);
    memcpy(data, content.data(), containerMemSize);
    SDL_UnmapGPUTransferBuffer(&device, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(&device);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUTransferBufferLocation src = {};
    src.transfer_buffer = transferBuffer;
    src.offset = 0;

    SDL_GPUBufferRegion dst = {};
    dst.buffer = buffer;
    dst.offset = 0;
    dst.size = containerMemSize;

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);

    SDL_ReleaseGPUTransferBuffer(&device, transferBuffer);
    return buffer;
}

template <ContiguousRange R>
SDL_GPUBuffer* CreateVertexBuffer(SDL_GPUDevice& device, R& vertices) {
    return CreateBuffer(device, vertices, SDL_GPU_BUFFERUSAGE_VERTEX);
}

template <ContiguousRange R>
SDL_GPUBuffer* CreateIndexBuffer(SDL_GPUDevice& device, R& indices) {
    return CreateBuffer(device, indices, SDL_GPU_BUFFERUSAGE_INDEX);
}

VertexLayoutInfo CreateVertexLayout(Renderer::EVertexLayout layoutType);

} // namespace Renderer