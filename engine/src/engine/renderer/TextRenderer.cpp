#include "engine/renderer/TextRenderer.h"
#include "engine/renderer/Vertex.h"

#include "engine/renderer/RendererUtils.h"
#include <SDL3/SDL_gpu.h>
#include <array>
#include <cstring>

namespace {
constexpr int kQuadVertexCount = 4;

constexpr std::array<Renderer::SVertex, kQuadVertexCount> kQuadVertices = {{
    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, -0.5f, 0.0f}, {1.0f, 0.0f}},
    {{0.5f, 0.5f, 0.0f}, {1.0f, 1.0f}},
    {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f}},
}};
constexpr uint32_t kQuadVerticesMemSize =
    static_cast<uint32_t>(kQuadVertexCount * sizeof(Renderer::SVertex));

constexpr int kQuadIndicesCount = 6;
constexpr std::array<uint16_t, kQuadIndicesCount> kQuadIndices = {{
    0,
    1,
    2,
    2,
    3,
    0,
}};
constexpr uint32_t kQuadIndicesMemSize =
    static_cast<uint32_t>(kQuadIndicesCount * sizeof(uint16_t));
} // namespace

namespace Renderer {

CTextRenderer::CTextRenderer(SDL_GPUDevice* device) : mDevice(device) {
    Initialize();
}

CTextRenderer::~CTextRenderer() {
    if (mQuadVertexBuffer) {
        SDL_ReleaseGPUBuffer(mDevice, mQuadVertexBuffer);
    }
    if (mQuadIndexBuffer) {
        SDL_ReleaseGPUBuffer(mDevice, mQuadIndexBuffer);
    }
}

void CTextRenderer::Initialize() {
    mQuadVertexBuffer = Renderer::CreateVertexBuffer(mDevice, kQuadVertices);
    mQuadIndexBuffer = Renderer::CreateIndexBuffer(mDevice, kQuadIndices);
}

SDL_GPUBuffer* CTextRenderer::GetQuadVertexBuffer() const {
    return mQuadVertexBuffer;
}

SDL_GPUBuffer* CTextRenderer::GetQuadIndexBuffer() const {
    return mQuadIndexBuffer;
}

SDL_GPUBuffer* CTextRenderer::CreateInstanceBuffer(size_t maxCharacters) {
    SDL_GPUBufferCreateInfo bufferInfo = {};
    bufferInfo.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
    bufferInfo.size = maxCharacters * sizeof(SCharacterInstance);

    return SDL_CreateGPUBuffer(mDevice, &bufferInfo);
}

void CTextRenderer::UpdateInstanceBuffer(
    SDL_GPUBuffer* buffer, const std::vector<SCharacterInstance>& instances) {

    if (instances.empty())
        return;

    size_t dataSize = instances.size() * sizeof(SCharacterInstance);

    SDL_GPUTransferBufferCreateInfo transferInfo = {};
    transferInfo.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
    transferInfo.size = dataSize;

    SDL_GPUTransferBuffer* transferBuffer =
        SDL_CreateGPUTransferBuffer(mDevice, &transferInfo);

    void* data = SDL_MapGPUTransferBuffer(mDevice, transferBuffer, false);
    memcpy(data, instances.data(), dataSize);
    SDL_UnmapGPUTransferBuffer(mDevice, transferBuffer);

    SDL_GPUCommandBuffer* uploadCmd = SDL_AcquireGPUCommandBuffer(mDevice);
    SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmd);

    SDL_GPUTransferBufferLocation src = {};
    src.transfer_buffer = transferBuffer;
    src.offset = 0;

    SDL_GPUBufferRegion dst = {};
    dst.buffer = buffer;
    dst.offset = 0;
    dst.size = dataSize;

    SDL_UploadToGPUBuffer(copyPass, &src, &dst, false);
    SDL_EndGPUCopyPass(copyPass);
    SDL_SubmitGPUCommandBuffer(uploadCmd);

    SDL_ReleaseGPUTransferBuffer(mDevice, transferBuffer);
}

} // namespace Renderer