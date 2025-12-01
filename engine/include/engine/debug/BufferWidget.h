#pragma once

#include "engine/debug/IOverlordItem.h"

namespace Vulkan {
class CBufferHandler;
} // namespace Vulkan

namespace Debug {

class CBufferWidget : public IOverlordItem {
public:
    CBufferWidget(Vulkan::CBufferHandler& bufferHandler);

    const char* GetName() const override;
    void Render() override;

private:
    Vulkan::CBufferHandler& mBufferHandler;
    int mBufferIndex;
};

} // namespace Debug