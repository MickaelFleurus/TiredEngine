#pragma once

namespace Vulkan {

class IBufferHandleWrapper {
public:
    virtual ~IBufferHandleWrapper() = default;
    virtual bool Upload() = 0;
    virtual bool Init(uint64_t bufferSize, VkBufferUsageFlags usage) = 0;
};

} // namespace Vulkan
