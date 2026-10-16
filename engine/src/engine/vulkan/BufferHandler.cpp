#include "engine/vulkan/BufferHandler.h"

#include <string>
#include <utility>
#include <vector>

#include "engine/core/DataTypes.h"
#include "engine/vulkan/Constants.h"
#include "engine/vulkan/GPUBuffer.h"


namespace {

constexpr auto kScreenQuadDataSize = sizeof(Core::SScreenQuadInstance);
constexpr int kScreenQuadBufferDefaultSize =
    Vulkan::kMaxSprites * kScreenQuadDataSize;

constexpr auto kWorldQuadDataSize = sizeof(Core::SWorldQuadInstance);
constexpr int kWorldQuadBufferDefaultSize =
    Vulkan::kMaxSprites * kWorldQuadDataSize;
constexpr auto kBufferUsageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                   VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

Vulkan::CGPUBuffer CreateVertexBuffer(const Vulkan::SContext& context) {

    constexpr VkDeviceSize vertexBufferSize = 1024 * 1024 * 10; // 10 MB
    return Vulkan::CGPUBuffer(context, vertexBufferSize,
                              VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                  VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                              false);
}

Vulkan::CGPUBuffer CreateIndexBuffer(const Vulkan::SContext& context) {

    constexpr VkDeviceSize indexBufferSize = 1024 * 1024 * 10; // 10 MB
    return Vulkan::CGPUBuffer(context, indexBufferSize,
                              VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
                                  VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                              false);
}

} // namespace
namespace Vulkan {

CBufferHandler::CBufferHandler(const Vulkan::SContext& context)
    : mContext(context)
    , mStagingBuffer(context)
    , mUiInstancesBuffer(context, kScreenQuadBufferDefaultSize,
                         kBufferUsageFlags)
    , mIndirectDrawBuffers(context)
    , mVertexBuffer(CreateVertexBuffer(context))
    , mIndexBuffer(CreateIndexBuffer(context))
    , mInstanceBuffer(CreateVertexBuffer(context))
    , mMaterialBuffer(CreateVertexBuffer(context))
    , mMeshInfoBuffer(CreateVertexBuffer(context))
    , mTransformBuffer(CreateVertexBuffer(context)) {
}

std::vector<std::pair<std::string, VkDeviceSize>>
CBufferHandler::GetBufferSizes() const {
    std::vector<std::pair<std::string, VkDeviceSize>> sizes{
        {"Vertex buffer", mVertexBuffer.GetSize()},
        {"Index buffer", mIndexBuffer.GetSize()},
        {"Instance buffer", mInstanceBuffer.GetSize()},
        {"Material buffer", mMaterialBuffer.GetSize()},
        {"Mesh info buffer", mMeshInfoBuffer.GetSize()},
        {"Transform buffer", mTransformBuffer.GetSize()},
        {"UI instances buffer", mUiInstancesBuffer.GetSize()},
    };

    const auto& drawGroups = mIndirectDrawBuffers.GetDrawGroups();
    for (size_t index = 0; index < drawGroups.size(); ++index) {
        const auto suffix = std::to_string(index);
        sizes.emplace_back("Indirect draw buffer " + suffix,
                           drawGroups[index].indirectBuffer.GetSize());
        sizes.emplace_back("Indirect count buffer " + suffix,
                           drawGroups[index].countBuffer.GetSize());
    }
    return sizes;
}

} // namespace Vulkan
