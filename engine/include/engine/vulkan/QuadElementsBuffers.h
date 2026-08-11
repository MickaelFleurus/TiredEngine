#pragma once

#include "engine/vulkan/GPUBuffer.h"
namespace Vulkan {

class CQuadElementsBuffers {
public:
    struct SQuadElementBuffer {
        CGPUBuffer buffer;
        void* mMappedPtr = nullptr;
        uint32_t count = 0;
    };

    CQuadElementsBuffers(const SContext& context);
    ~CQuadElementsBuffers();

private:
    SQuadElementBuffer mUiGlyphBuffer, mWorldGlyphBuffer;
    SQuadElementBuffer mUiSpriteBuffer, mWorldSpriteBuffer;
    const SContext& mContext;
};

} // namespace Vulkan
