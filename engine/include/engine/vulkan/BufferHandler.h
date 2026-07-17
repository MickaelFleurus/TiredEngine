#pragma once

#include <memory>
#include <unordered_map>

#include "engine/core/DataTypes.h"
#include "engine/vulkan/BufferHandleWrapper.h"

namespace Renderer {
class CMemoryAllocator;
}

namespace Vulkan {

struct SContext;

class CBufferHandler {
public:
    CBufferHandler(const Vulkan::SContext& context,
                   Renderer::CMemoryAllocator& memoryAllocator);
    ~CBufferHandler();

    void Upload();

    template <typename T>
    CBufferHandleWrapper<T>& Get(int index) {
        auto* wrapper = mBufferWrappers[index].get();
        auto* typedWrapper = static_cast<CBufferHandleWrapper<T>*>(wrapper);
        return *typedWrapper;
    }

    template <typename T>
    CBufferHandleWrapper<T>* CreateTemp() {
        return new CBufferHandleWrapper<T>(mContext, mMemoryAllocator);
    }

private:
    const Vulkan::SContext& mContext;
    Renderer::CMemoryAllocator& mMemoryAllocator;

    std::unordered_map<int, std::unique_ptr<IBufferHandleWrapper>>
        mBufferWrappers;
};
} // namespace Vulkan
