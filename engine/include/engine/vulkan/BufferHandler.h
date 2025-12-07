#pragma once

#include "engine/core/DataTypes.h"
#include "engine/vulkan/BufferHandleWrapper.h"

#include <memory>
#include <typeindex>
#include <unordered_map>

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

    void Upload();

    template <typename T>
    CBufferHandleWrapper<T>& Get() {
        auto* wrapper = mBufferWrappers[typeid(T)].get();
        auto* typedWrapper = static_cast<CBufferHandleWrapper<T>*>(wrapper);
        return *typedWrapper;
    }

    template <typename T>
    CBufferHandleWrapper<T>* CreateTemp() {
        return new CBufferHandleWrapper<T>(mVulkanContext, mMemoryAllocator);
    }

private:
    const Vulkan::CVulkanContext& mVulkanContext;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    std::unordered_map<std::type_index, std::unique_ptr<IBufferHandleWrapper>>
        mBufferWrappers;
};
} // namespace Vulkan