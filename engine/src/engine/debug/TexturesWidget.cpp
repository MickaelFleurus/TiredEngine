#include "engine/debug/TexturesWidget.h"

#include <imgui_impl_vulkan.h>

#include "engine/debug/TextureImGuiContainer.h"
#include "engine/renderer/TextureManager.h"

namespace Debug {
CTexturesWidget::CTexturesWidget(CTextureImGuiContainer& textureContainer)
    : mTextureContainer(textureContainer) {
}

const char* CTexturesWidget::GetName() const {
    return "Textures";
}

void CTexturesWidget::Render() {
    if (ImGui::Begin("Textures", &mVisible)) {
        for (const auto& info : mTextureContainer.Get()) {
            ImGui::Text("Name: %s", info.name.c_str());
            ImGui::Text("Texture index: %i", info.id);
            ImGui::Text("VkImage: 0x%llX", (uint64_t)info.vkInfo.image);
            ImGui::Image(info.imguiTexId, ImVec2(128, 128));

            ImGui::Separator();
        }
        ImGui::End();
    }
}

} // namespace Debug
