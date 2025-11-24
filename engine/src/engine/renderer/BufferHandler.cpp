#include "engine/renderer/BufferHandler.h"
#include "engine/renderer/MemoryAllocator.h"

#include "engine/vulkan/IVulkanContext.h"

#include "engine/utils/Logger.h"

namespace Renderer {

CBufferHandler::CBufferHandler(
    const Vulkan::IVulkanContextGetter& vulkanContext,
    Renderer::CMemoryAllocator& memoryAllocator)
    : mVulkanContext(vulkanContext), mMemoryAllocator(memoryAllocator) {
}

CBufferHandle* CBufferHandler::CreateBuffer(int size, int dataSize,
                                            VkBufferUsageFlags usage) {
    const int index = static_cast<int>(mBuffers.size());

    CBufferHandle bufferHandle{mVulkanContext, mMemoryAllocator, *this, index};
    if (!bufferHandle.Init(size, dataSize, usage)) {
        LOG_ERROR("Failed to create buffer handle!");
        return nullptr;
    }

    mBuffers.emplace_back(std::move(bufferHandle));
    return &mBuffers.back();
}

} // namespace Renderer