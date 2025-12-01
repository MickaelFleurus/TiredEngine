#pragma once

#include "engine/core/DataTypes.h"
#include "engine/utils/Logger.h"
#include "engine/vulkan/BufferHandle.h"
#include "engine/vulkan/BufferHandleWrapper.h"

#include <memory>
#include <typeindex>
#include <vector>

namespace Renderer {
class CMemoryAllocator;
}

namespace Vulkan {

class CVulkanContext;

class CBufferHandler {
public:
    CBufferHandler(const Vulkan::CVulkanContext& vulkanContext,
                   Renderer::CMemoryAllocator& memoryAllocator);
    ~CBufferHandler();

    CBufferHandle* CreateBuffer(int dataSize, int size,
                                VkBufferUsageFlags usage);
    void Free();
    void Upload();

    template <typename T>
    CBufferHandleWrapper<T>& GetWrapper() {
        auto* wrapper = mBufferWrappers[typeid(T)].get();
        auto* typedWrapper = static_cast<CBufferHandleWrapper<T>*>(wrapper);
        return *typedWrapper;
    }

    void FreeBuffer(CBufferHandle* bufferHandle);

    template <typename T>
    void FreeBuffer() {
        FreeBuffer(mBufferWrappers[typeid(T)]->GetBuffer());
    }

    template <typename T>
    void Upload() {
        auto* wrapper = mBufferWrappers[typeid(T)].get();
        if (!wrapper->Upload()) {
            LOG_ERROR("Failed to upload buffer data!");
        }
    }

private:
    const Vulkan::CVulkanContext& mVulkanContext;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    std::vector<std::unique_ptr<CBufferHandle>> mBuffers;
    std::unordered_map<std::type_index, std::unique_ptr<IBufferHandleWrapper>>
        mBufferWrappers;
};
} // namespace Vulkan