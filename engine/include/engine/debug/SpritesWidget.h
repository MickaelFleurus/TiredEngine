#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.h>

#include "engine/debug/IOverlordItem.h"

namespace Renderer {
class CSpriteManager;
class CTextureManager;
} // namespace Renderer

namespace Debug {
class CSpritesWidget : public IOverlordItem {
public:
    CSpritesWidget(Renderer::CSpriteManager& spriteManager,
                   Renderer::CTextureManager& textureManager);
    ~CSpritesWidget() override;

    void Render() override;
    const char* GetName() const override;

private:
    struct STexturePreview {
        VkImageView imageView = VK_NULL_HANDLE;
        VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
    };

    Renderer::CSpriteManager& mSpriteManager;
    Renderer::CTextureManager& mTextureManager;
    std::unordered_map<uint64_t, STexturePreview> mPreviews;
    std::string mFilter;
};
} // namespace Debug
