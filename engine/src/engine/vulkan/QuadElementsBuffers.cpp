#include "engine/vulkan/QuadElementsBuffers.h"

#include "engine/core/DataTypes.h"
#include "engine/vulkan/Constants.h"
#include "engine/vulkan/GPUBuffer.h"
#include "engine/vulkan/VulkanContext.h"

namespace {

constexpr auto kScreenQuadDataSize = sizeof(Core::ScreenQuadInstance);
constexpr int kScreenQuadBufferDefaultSize =
    Vulkan::kMaxSprites * kScreenQuadDataSize;

constexpr auto kWorldQuadDataSize = sizeof(Core::WorldQuadInstance);
constexpr int kWorldQuadBufferDefaultSize =
    Vulkan::kMaxSprites * kWorldQuadDataSize;
constexpr auto kBufferUsageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT |
                                   VK_BUFFER_USAGE_TRANSFER_DST_BIT |
                                   VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

void FigureMappedPtr(const Vulkan::SContext& context,
                     Vulkan::CQuadElementsBuffers::SQuadElementBuffer& buffer) {
    VmaAllocationInfo info;
    vmaGetAllocationInfo(context.vmaAllocator, buffer.buffer.GetAllocation(),
                         &info);
    buffer.mMappedPtr = info.pMappedData;
}
} // namespace

namespace Vulkan {

CQuadElementsBuffers::CQuadElementsBuffers(const SContext& context)
    : mContext(context)
    , mUiGlyphBuffer{CGPUBuffer(context, kScreenQuadBufferDefaultSize,
                                kBufferUsageFlags, true)}
    , mWorldGlyphBuffer{CGPUBuffer(context, kWorldQuadBufferDefaultSize,
                                   kBufferUsageFlags, true)}

    , mUiSpriteBuffer{CGPUBuffer(context, kScreenQuadBufferDefaultSize,
                                 kBufferUsageFlags, true)}
    , mWorldSpriteBuffer{CGPUBuffer(context, kWorldQuadBufferDefaultSize,
                                    kBufferUsageFlags, true)} {
    FigureMappedPtr(context, mUiGlyphBuffer);
    FigureMappedPtr(context, mWorldGlyphBuffer);
    FigureMappedPtr(context, mUiSpriteBuffer);
    FigureMappedPtr(context, mWorldSpriteBuffer);
}
} // namespace Vulkan
