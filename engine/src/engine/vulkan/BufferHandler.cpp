#include "engine/vulkan/BufferHandler.h"

#include "engine/core/DataTypes.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/vulkan/Constants.h"

namespace {
constexpr auto kVertexStructSize = sizeof(Core::SVertex);
constexpr int kVertexBufferDefaultSize =
    Vulkan::kVertexAmountPerBuffer * kVertexStructSize;
constexpr auto kVertexBufferUsage =
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr auto kIndexStructSize = sizeof(Core::IndexType);
constexpr int kIndexBufferDefaultSize =
    Vulkan::kIndexAmountPerBuffer * kIndexStructSize;
constexpr auto kIndexBufferUsage =
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr auto kInstanceStructSize = sizeof(Core::SInstanceData);
constexpr int kInstanceBufferDefaultSize =
    Vulkan::kInstanceAmountPerBuffer * kInstanceStructSize;
constexpr auto kInstanceBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                      VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

constexpr auto kInstanceInfoStructSize = sizeof(Core::SIndirectDrawCommand);
constexpr int kInstanceInfoBufferDefaultSize =
    Vulkan::kInstanceInfoAmountPerBuffer * kInstanceInfoStructSize;
constexpr auto kInstanceInfoBufferUsage =
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr auto kTextInstanceStructSize = sizeof(Core::SUIInstanceData);
constexpr int kTextInstanceBufferDefaultSize =
    Vulkan::kTextInstanceAmountPerBuffer * kTextInstanceStructSize;
constexpr auto kTextInstanceBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                          VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                          VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

constexpr auto kUIVertexStructSize = sizeof(Core::SUIVertex);
constexpr int kUIVertexBufferDefaultSize =
    Vulkan::kVertexAmountPerBuffer * kUIVertexStructSize;
constexpr auto kUIVertexBufferUsage =
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr auto kUIIndexStructSize = sizeof(Core::TextIndexType);
constexpr int kUIIndexBufferDefaultSize =
    Vulkan::kIndexAmountPerBuffer * kUIIndexStructSize;
constexpr auto kUIIndexBufferUsage =
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

} // namespace

namespace Vulkan {

CBufferHandler::CBufferHandler(const Vulkan::CVulkanContext& vulkanContext,
                               Renderer::CMemoryAllocator& memoryAllocator)
    : mVulkanContext(vulkanContext), mMemoryAllocator(memoryAllocator) {

    mBufferWrappers[kVerticesBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::SVertex>>(mVulkanContext,
                                                              mMemoryAllocator);
    mBufferWrappers[kVerticesBufferIndex]->Init(kVertexBufferDefaultSize,
                                                kVertexBufferUsage);

    mBufferWrappers[kIndicesBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::IndexType>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[kIndicesBufferIndex]->Init(kIndexBufferDefaultSize,
                                               kIndexBufferUsage);
    mBufferWrappers[kInstanceBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::SInstanceData>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[kInstanceBufferIndex]->Init(kInstanceBufferDefaultSize,
                                                kInstanceBufferUsage);

    mBufferWrappers[kInstanceInfoBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::SIndirectDrawCommand>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[kInstanceInfoBufferIndex]->Init(
        kInstanceInfoBufferDefaultSize, kInstanceInfoBufferUsage);

    mBufferWrappers[kTextInstanceBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::SUIInstanceData>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[kTextInstanceBufferIndex]->Init(
        kTextInstanceBufferDefaultSize, kTextInstanceBufferUsage);

    mBufferWrappers[kUIVerticesBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::SUIVertex>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[kUIVerticesBufferIndex]->Init(kUIVertexBufferDefaultSize,
                                                  kUIVertexBufferUsage);
    mBufferWrappers[kUIIndicesBufferIndex] =
        std::make_unique<CBufferHandleWrapper<Core::TextIndexType>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[kUIIndicesBufferIndex]->Init(kUIIndexBufferDefaultSize,
                                                 kUIIndexBufferUsage);
}

CBufferHandler::~CBufferHandler() = default;

} // namespace Vulkan