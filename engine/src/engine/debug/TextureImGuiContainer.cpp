#include "engine/debug/TextureImGuiContainer.h"

#include <imgui_impl_vulkan.h>

#include "engine/renderer/TextureManager.h"

namespace Debug {
CTextureImGuiContainer::CTextureImGuiContainer(
    Renderer::CTextureManager& textureManager)
    : mTextureManager(textureManager) {
}
CTextureImGuiContainer::~CTextureImGuiContainer() {
    for (const auto& info : mImGuiTextureIds) {
        ImGui_ImplVulkan_RemoveTexture(
            reinterpret_cast<VkDescriptorSet>(info.imguiTexId));
    }
}

const std::vector<CTextureImGuiContainer::SImGuiTextureInfo>&
CTextureImGuiContainer::Get() {
    if (mImGuiTextureIds.empty()) {
        for (const auto& [name, index] :
             mTextureManager.GetAllTextureIndices()) {
            CTextureImGuiContainer::SImGuiTextureInfo info{
                .name = name,
                .id = index,
                .vkInfo = mTextureManager.GetTexture(index)};

            const auto& tex = mTextureManager.GetTexture(index);
            info.imguiTexId =
                reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(
                    tex.sampler, tex.imageView,
                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
            mImGuiTextureIds.push_back(info);
        }
    }
    return mImGuiTextureIds;
}
} // namespace Debug
