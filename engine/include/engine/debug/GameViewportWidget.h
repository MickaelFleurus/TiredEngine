#pragma once

#include <vulkan/vulkan.h>

#include "engine/debug/IOverlordItem.h"

namespace Vulkan {
class CViewportTarget;
}

namespace Debug {
class CGameViewportWidget : public IOverlordItem {
public:
    explicit CGameViewportWidget(Vulkan::CViewportTarget& target);
    ~CGameViewportWidget() override;

    void Render() override;
    const char* GetName() const override;

private:
    Vulkan::CViewportTarget& mTarget;
    VkImageView mImageView = VK_NULL_HANDLE;
    VkDescriptorSet mDescriptorSet = VK_NULL_HANDLE;
};
} // namespace Debug
