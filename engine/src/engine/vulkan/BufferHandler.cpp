#include "engine/vulkan/BufferHandler.h"

#include "engine/core/DataTypes.h"
#include "engine/renderer/MemoryAllocator.h"
#include "engine/vulkan/Constants.h"

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

    mBufferWrappers[typeid(Core::SVertex)] =
        std::make_unique<CBufferHandleWrapper<Core::SVertex>>(mVulkanContext,
                                                              mMemoryAllocator);
    mBufferWrappers[typeid(Core::SVertex)]->Init(kVertexBufferDefaultSize,
                                                 kVertexBufferUsage);

    mBufferWrappers[typeid(Core::IndexType)] =
        std::make_unique<CBufferHandleWrapper<Core::IndexType>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[typeid(Core::IndexType)]->Init(kIndexBufferDefaultSize,
                                                   kIndexBufferUsage);

    mBufferWrappers[typeid(Core::SInstanceData)] =
        std::make_unique<CBufferHandleWrapper<Core::SInstanceData>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[typeid(Core::SInstanceData)]->Init(
        kInstanceBufferDefaultSize, kInstanceBufferUsage);

    mBufferWrappers[typeid(Core::SIndirectDrawCommand)] =
        std::make_unique<CBufferHandleWrapper<Core::SIndirectDrawCommand>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[typeid(Core::SIndirectDrawCommand)]->Init(
        kInstanceInfoBufferDefaultSize, kInstanceInfoBufferUsage);

    mBufferWrappers[typeid(Core::STextInstanceData)] =
        std::make_unique<CBufferHandleWrapper<Core::STextInstanceData>>(
            mVulkanContext, mMemoryAllocator);
    mBufferWrappers[typeid(Core::STextInstanceData)]->Init(
        kTextInstanceBufferDefaultSize, kTextInstanceBufferUsage);
}

CBufferHandler::~CBufferHandler() = default;

} // namespace Vulkan