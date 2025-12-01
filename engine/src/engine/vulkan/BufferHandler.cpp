#include "engine/vulkan/BufferHandler.h"

#include "engine/core/DataTypes.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/Constants.h"
#include "engine/vulkan/VulkanContext.h"

namespace {
constexpr int kVerticesBufferIndex = 0;
constexpr auto kVertexStructSize = sizeof(Core::SVertex);
constexpr int kVertexBufferDefaultSize =
    Vulkan::kVertexAmountPerBuffer * kVertexStructSize;
constexpr auto kVertexBufferUsage =
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr int kIndicesBufferIndex = 1;
constexpr auto kIndexStructSize = sizeof(Core::IndexType);
constexpr int kIndexBufferDefaultSize =
    Vulkan::kIndexAmountPerBuffer * kIndexStructSize;
constexpr auto kIndexBufferUsage =
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr int kInstanceBufferIndex = 2;
constexpr auto kInstanceStructSize = sizeof(Core::SInstanceData);
constexpr int kInstanceBufferDefaultSize =
    Vulkan::kInstanceAmountPerBuffer * kInstanceStructSize;
constexpr auto kInstanceBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                      VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

constexpr int kInstanceInfoBufferIndex = 3;
constexpr auto kInstanceInfoStructSize = sizeof(Core::SIndirectDrawCommand);
constexpr int kInstanceInfoBufferDefaultSize =
    Vulkan::kInstanceInfoAmountPerBuffer * kInstanceInfoStructSize;
constexpr auto kInstanceInfoBufferUsage =
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr int kTextInstanceBufferIndex = 4;
constexpr auto kTextInstanceStructSize = sizeof(Core::STextInstanceData);
constexpr int kTextInstanceBufferDefaultSize =
    Vulkan::kTextInstanceAmountPerBuffer * kTextInstanceStructSize;
constexpr auto kTextInstanceBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                          VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

} // namespace

namespace Vulkan {

CBufferHandler::CBufferHandler(const Vulkan::CVulkanContext& vulkanContext,
                               Renderer::CMemoryAllocator& memoryAllocator)
    : mVulkanContext(vulkanContext), mMemoryAllocator(memoryAllocator) {
    mBuffers.reserve(5);

    mBufferWrappers[typeid(Core::SVertex)] =
        std::make_unique<CBufferHandleWrapper<Core::SVertex>>(CreateBuffer(
            kVertexStructSize, kVertexBufferDefaultSize, kVertexBufferUsage));

    mBufferWrappers[typeid(Core::IndexType)] =
        std::make_unique<CBufferHandleWrapper<Core::IndexType>>(CreateBuffer(
            kIndexStructSize, kIndexBufferDefaultSize, kIndexBufferUsage));

    mBufferWrappers[typeid(Core::SInstanceData)] =
        std::make_unique<CBufferHandleWrapper<Core::SInstanceData>>(
            CreateBuffer(kInstanceStructSize, kInstanceBufferDefaultSize,
                         kInstanceBufferUsage));

    mBufferWrappers[typeid(Core::SIndirectDrawCommand)] =
        std::make_unique<CBufferHandleWrapper<Core::SIndirectDrawCommand>>(
            CreateBuffer(kInstanceInfoStructSize,
                         kInstanceInfoBufferDefaultSize,
                         kInstanceInfoBufferUsage));

    mBufferWrappers[typeid(Core::STextInstanceData)] =
        std::make_unique<CBufferHandleWrapper<Core::STextInstanceData>>(
            CreateBuffer(kTextInstanceStructSize,
                         kTextInstanceBufferDefaultSize,
                         kTextInstanceBufferUsage));
}

CBufferHandler::~CBufferHandler() = default;

void CBufferHandler::Free() {
    for (auto& buffer : mBuffers) {
        FreeBuffer(buffer.get());
    }
}

void CBufferHandler::FreeBuffer(CBufferHandle* bufferHandle) {
    bufferHandle->FreeRanges();
}

void CBufferHandler::Upload() {
    for (auto& wrapperPair : mBufferWrappers) {
        if (!wrapperPair.second->Upload()) {
            LOG_ERROR("Failed to upload buffer data!");
        }
    }
}

CBufferHandle* CBufferHandler::CreateBuffer(int dataSize, int size,
                                            VkBufferUsageFlags usage) {
    auto bufferHandle = std::make_unique<CBufferHandle>(
        mVulkanContext, mMemoryAllocator, *this);
    if (!bufferHandle->Init(dataSize, size, usage)) {
        LOG_ERROR("Failed to create buffer handle!");
        return nullptr;
    }

    mBuffers.push_back(std::move(bufferHandle));
    return mBuffers.back().get();
}

} // namespace Vulkan