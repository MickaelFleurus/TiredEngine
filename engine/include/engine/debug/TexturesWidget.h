#pragma once

#include <cstdint>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "engine/debug/IOverlordItem.h"

namespace Renderer {
class CTextureManager;
}

namespace Debug {
class CTexturesWidget : public IOverlordItem {
public:
    explicit CTexturesWidget(Renderer::CTextureManager& textureManager);
    ~CTexturesWidget() override;

    void Render() override;
    const char* GetName() const override;

private:
    struct STexturePreview {
        VkImageView imageView = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };

    Renderer::CTextureManager& mTextureManager;
    std::unordered_map<uint64_t, STexturePreview> mPreviews;
};
} // namespace Debug
