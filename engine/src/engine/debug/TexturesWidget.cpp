#include "engine/debug/TexturesWidget.h"

#include "engine/renderer/TextureManager.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

namespace Debug {
CTexturesWidget::CTexturesWidget(Renderer::CTextureManager& textureManager)
    : mTextureManager(textureManager) {
}

CTexturesWidget::~CTexturesWidget() {
    for (const auto& [index, imguiTextureID] : mImGuiTextureIDs) {
        ImGui_ImplVulkan_RemoveTexture(
            reinterpret_cast<VkDescriptorSet>(imguiTextureID));
    }
}

const char* CTexturesWidget::GetName() const {
    return "Textures";
}

void CTexturesWidget::Render() {
    bool isVisible = IsVisible();
    if (ImGui::Begin("Textures", &isVisible)) {
        for (const auto& [name, index] :
             mTextureManager.GetAllTextureIndices()) {
            ImGui::Text("Name: %s", name.c_str());
            const auto& tex = mTextureManager.GetTexture(index);
            ImGui::Text("VkImage: 0x%llX", (uint64_t)tex.image);

            if (!mImGuiTextureIDs.contains(index)) {
                ImTextureID imguiTextureID =
                    reinterpret_cast<ImTextureID>(ImGui_ImplVulkan_AddTexture(
                        tex.sampler, tex.imageView,
                        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
                mImGuiTextureIDs.emplace(index, imguiTextureID);
            }
            ImGui::Image(mImGuiTextureIDs.at(index), ImVec2(128, 128));

            ImGui::Separator();
        }
        ImGui::End();
    }
}

} // namespace Debug