#include "engine/debug/GameViewportWidget.h"

#include <algorithm>
#include <cstdint>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "engine/vulkan/ViewportTarget.h"

namespace Debug {
CGameViewportWidget::CGameViewportWidget(Vulkan::CViewportTarget& target)
    : mTarget(target) {
    SetVisible(true);
}

CGameViewportWidget::~CGameViewportWidget() {
    if (mDescriptorSet != VK_NULL_HANDLE) {
        ImGui_ImplVulkan_RemoveTexture(mDescriptorSet);
    }
}

void CGameViewportWidget::Render() {
    if (!ImGui::Begin("Game View", &mVisible)) {
        ImGui::End();
        return;
    }

    const VkImageView imageView = mTarget.GetImageView();
    if (imageView != mImageView) {
        if (mDescriptorSet != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(mDescriptorSet);
            mDescriptorSet = VK_NULL_HANDLE;
        }
        mImageView = imageView;
        if (mImageView != VK_NULL_HANDLE) {
            mDescriptorSet = ImGui_ImplVulkan_AddTexture(
                mImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        }
    }

    const ImVec2 available = ImGui::GetContentRegionAvail();
    const ImGuiIO& io = ImGui::GetIO();
    const auto targetExtent = mTarget.GetExtent();
    if (available.x > 0.0f && available.y > 0.0f && targetExtent.width > 0 &&
        targetExtent.height > 0) {
        const float aspect = static_cast<float>(targetExtent.width) /
                             static_cast<float>(targetExtent.height);
        const float scale = std::min(available.x / targetExtent.width,
                                     available.y / targetExtent.height);
        const ImVec2 imageSize{targetExtent.width * scale,
                               targetExtent.height * scale};
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() +
                             (available.x - imageSize.x) * 0.5f);
        if (mDescriptorSet != VK_NULL_HANDLE) {
            const auto textureId = static_cast<ImTextureID>(
                reinterpret_cast<uintptr_t>(mDescriptorSet));
            ImGui::Image(textureId, imageSize);
        }

        const float scaleX = std::max(io.DisplayFramebufferScale.x, 1.0f);
        const float scaleY = std::max(io.DisplayFramebufferScale.y, 1.0f);
        const auto requestedWidth =
            static_cast<uint32_t>(std::max(1.0f, available.x * scaleX));
        const auto requestedHeight =
            static_cast<uint32_t>(std::max(1.0f, available.y * scaleY));
        const float targetAspect =
            static_cast<float>(targetExtent.width) / targetExtent.height;
        uint32_t renderWidth = requestedWidth;
        uint32_t renderHeight = requestedHeight;
        if (static_cast<float>(requestedWidth) / requestedHeight >
            targetAspect) {
            renderWidth = static_cast<uint32_t>(requestedHeight * targetAspect);
        } else {
            renderHeight = static_cast<uint32_t>(requestedWidth / targetAspect);
        }
        mTarget.RequestResize(renderWidth, renderHeight);
        ImGui::TextDisabled("%u x %u  |  %.2f:1", targetExtent.width,
                            targetExtent.height, aspect);
    }
    ImGui::End();
}

const char* CGameViewportWidget::GetName() const {
    return "Game View";
}
} // namespace Debug
