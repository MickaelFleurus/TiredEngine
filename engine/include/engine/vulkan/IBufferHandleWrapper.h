#pragma once

namespace Vulkan {

class IBufferHandleWrapper {
public:
    virtual ~IBufferHandleWrapper() = default;
    virtual bool Upload() = 0;
    virtual bool Clear() = 0;
    virtual CBufferHandle* GetBuffer() const = 0;
};

} // namespace Vulkan