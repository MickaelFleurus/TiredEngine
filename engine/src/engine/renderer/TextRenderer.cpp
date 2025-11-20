#include "engine/renderer/TextRenderer.h"
#include "engine/renderer/Vertex.h"

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

CTextRenderer::CTextRenderer(VkDevice device,
                             VkPhysicalDeviceMemoryProperties memProperties)
    : mDevice(device), mMemProperties(memProperties) {
    Initialize();
}

void CTextRenderer::Initialize() {
    mQuadVertexBuffer = std::move(Renderer::CreateAndFillVertexBuffer(
        mDevice, kQuadVertices, mMemProperties));
    mQuadIndexBuffer = std::move(Renderer::CreateAndFillIndexBuffer(mDevice, kQuadIndices,
                                                          mMemProperties));
}

VkBuffer CTextRenderer::GetQuadVertexBuffer() const {
    return mQuadVertexBuffer.buffer;
}

VkBuffer CTextRenderer::GetQuadIndexBuffer() const {
    return mQuadIndexBuffer.buffer;
}

VulkanBuffer CTextRenderer::CreateInstanceBuffer(size_t maxCharacters) {
    return Renderer::CreateBuffer(
        mDevice,
        static_cast<uint32_t>(maxCharacters * sizeof(SCharacterInstance)),
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, mMemProperties);
}

void CTextRenderer::UpdateInstanceBuffer(
    VulkanBuffer& buffer, const std::vector<SCharacterInstance>& instances) {

    if (instances.empty())
        return;

    size_t dataSize = instances.size() * sizeof(SCharacterInstance);

    Renderer::FillBuffer(mDevice, buffer, instances.data(),
                         static_cast<uint32_t>(dataSize));
}

} // namespace Renderer