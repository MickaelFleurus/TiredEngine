#include "engine/renderer/BufferHandler.h"
#include "engine/renderer/MemoryAllocator.h"

#include "engine/renderer/Constants.h"
#include "engine/vulkan/VulkanContext.h"

#include "engine/renderer/IndexesBufferHandleWrapper.h"
#include "engine/renderer/VertexBufferHandleWrapper.h"

#include "engine/renderer/DataTypes.h"

#include "engine/utils/Logger.h"

namespace {
constexpr int kVerticesBufferIndex = 0;
constexpr auto kVertexStructSize = sizeof(Renderer::SVertex);
constexpr int kVertexBufferDefaultSize =
    Renderer::kVertexAmountPerBuffer * kVertexStructSize;
constexpr auto kVertexBufferUsage =
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr int kIndicesBufferIndex = 1;
constexpr auto kIndexStructSize = sizeof(uint32_t);
constexpr int kIndexBufferDefaultSize =
    Renderer::kIndexAmountPerBuffer * kIndexStructSize;
constexpr auto kIndexBufferUsage =
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

constexpr int kInstanceBufferIndex = 2;
constexpr auto kInstanceStructSize = sizeof(Renderer::SInstanceData);
constexpr int kInstanceBufferDefaultSize =
    Renderer::kInstanceAmountPerBuffer * kInstanceStructSize;
constexpr auto kInstanceBufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                      VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                      VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;

constexpr int kInstanceInfoBufferIndex = 3;
constexpr auto kInstanceInfoStructSize = sizeof(VkDrawIndexedIndirectCommand);
constexpr int kInstanceInfoBufferDefaultSize =
    Renderer::kInstanceInfoAmountPerBuffer * kInstanceInfoStructSize;
constexpr auto kInstanceInfoBufferUsage =
    VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

} // namespace

namespace Renderer {

CBufferHandler::CBufferHandler(const Vulkan::CVulkanContext& vulkanContext,
                               Renderer::CMemoryAllocator& memoryAllocator)
    : mVulkanContext(vulkanContext), mMemoryAllocator(memoryAllocator) {
    mBuffers.reserve(3);
    mVerticesBufferHandle =
        std::make_unique<CVertexBufferHandleWrapper>(CreateBuffer(
            kVertexStructSize, kVertexBufferDefaultSize, kVertexBufferUsage));
    mIndexesBufferHandle =
        std::make_unique<CIndexesBufferHandleWrapper>(CreateBuffer(
            kIndexStructSize, kIndexBufferDefaultSize, kIndexBufferUsage));
    CreateBuffer(kInstanceStructSize, kInstanceBufferDefaultSize,
                 kInstanceBufferUsage);
    CreateBuffer(kInstanceInfoStructSize, kInstanceInfoBufferDefaultSize,
                 kInstanceInfoBufferUsage);
}

CBufferHandler::~CBufferHandler() = default;

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

CVertexBufferHandleWrapper& CBufferHandler::GetVerticesBufferHandle() {
    return *mVerticesBufferHandle;
}

CIndexesBufferHandleWrapper& CBufferHandler::GetIndexesBufferHandle() {
    return *mIndexesBufferHandle;
}

CBufferHandle& CBufferHandler::GetInstanceBufferHandle() {
    return *mBuffers.at(kInstanceBufferIndex);
}

CBufferHandle& CBufferHandler::GetInstancesInfoBufferHandle() {
    return *mBuffers.at(kInstanceBufferIndex);
}

void CBufferHandler::BindBuffers(VkCommandBuffer commandBuffer) {
    VkBuffer vertexBuffers[] = {mBuffers.at(kVerticesBufferIndex)->GetBuffer(),
                                mBuffers.at(kInstanceBufferIndex)->GetBuffer()};
    VkDeviceSize offsets[] = {0, 0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offsets);

    VkBuffer indexBuffer = mBuffers.at(kIndicesBufferIndex)->GetBuffer();
    vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
}

void CBufferHandler::FreeBuffer(CBufferHandle* bufferHandle) {
    auto it = std::remove_if(
        mBuffers.begin(), mBuffers.end(),
        [bufferHandle](const std::unique_ptr<CBufferHandle>& ptr) {
            return ptr.get() == bufferHandle;
        });
    if (it != mBuffers.end()) {
        mBuffers.erase(it, mBuffers.end());
    } else {
        LOG_WARNING("Trying to free a non-existing buffer handle!");
    }
}

} // namespace Renderer