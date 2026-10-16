#include "engine/debug/TexturesWidget.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#include "engine/renderer/TextureManager.h"

namespace Debug {
CTexturesWidget::CTexturesWidget(Renderer::CTextureManager& textureManager)
    : mTextureManager(textureManager) {
}

CTexturesWidget::~CTexturesWidget() {
    for (const auto& [index, preview] : mPreviews) {
        if (preview.descriptorSet != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(preview.descriptorSet);
        }
    }
}

void CTexturesWidget::Render() {
    if (!ImGui::Begin("Loaded Textures", &mVisible)) {
        ImGui::End();
        return;
    }

    const auto& textureIndices = mTextureManager.GetAllTextureIndices();
    std::vector<std::pair<std::string, uint64_t>> textures;
    textures.reserve(textureIndices.size());
    for (const auto& [name, index] : textureIndices) {
        textures.emplace_back(name.GetName(), index);
    }
    std::ranges::sort(textures, {}, &std::pair<std::string, uint64_t>::first);

    std::unordered_map<uint64_t, VkImageView> currentViews;
    for (const auto& [name, index] : textures) {
        const auto& texture = mTextureManager.GetTexture(index);
        if (texture.imageView != VK_NULL_HANDLE) {
            currentViews.emplace(index, texture.imageView);
        }
    }

    for (auto it = mPreviews.begin(); it != mPreviews.end();) {
        const auto current = currentViews.find(it->first);
        if (current == currentViews.end() ||
            current->second != it->second.imageView) {
            if (it->second.descriptorSet != VK_NULL_HANDLE) {
                ImGui_ImplVulkan_RemoveTexture(it->second.descriptorSet);
            }
            it = mPreviews.erase(it);
        } else {
            ++it;
        }
    }

    for (const auto& [index, imageView] : currentViews) {
        if (!mPreviews.contains(index)) {
            mPreviews.emplace(
                index,
                STexturePreview{
                    .imageView = imageView,
                    .descriptorSet = ImGui_ImplVulkan_AddTexture(
                        imageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)});
        }
    }

    if (textures.empty()) {
        ImGui::TextDisabled("No textures are loaded.");
    } else {
        for (const auto& [name, index] : textures) {
            const auto& texture = mTextureManager.GetTexture(index);
            ImGui::PushID(static_cast<int>(index));
            ImGui::TextUnformatted(name.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("%u x %u  |  #%llu", texture.width,
                                texture.height,
                                static_cast<unsigned long long>(index));

            const auto preview = mPreviews.find(index);
            if (preview != mPreviews.end() &&
                preview->second.descriptorSet != VK_NULL_HANDLE) {
                const float maxWidth = ImGui::GetContentRegionAvail().x;
                const float scale = std::min(
                    1.0f, maxWidth / static_cast<float>(texture.width));
                const ImVec2 size{static_cast<float>(texture.width) * scale,
                                  static_cast<float>(texture.height) * scale};
                const auto textureId = static_cast<ImTextureID>(
                    reinterpret_cast<uintptr_t>(preview->second.descriptorSet));
                ImGui::Image(textureId, size);
            }
            ImGui::Separator();
            ImGui::PopID();
        }
    }

    ImGui::End();
}

const char* CTexturesWidget::GetName() const {
    return "Loaded Textures";
}
} // namespace Debug
