#include "engine/debug/SpritesWidget.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_stdlib.h>

#include "engine/renderer/SpriteManager.h"
#include "engine/renderer/TextureManager.h"

namespace Debug {
CSpritesWidget::CSpritesWidget(Renderer::CSpriteManager& spriteManager,
                               Renderer::CTextureManager& textureManager)
    : mSpriteManager(spriteManager), mTextureManager(textureManager) {
}

CSpritesWidget::~CSpritesWidget() {
    for (const auto& [index, preview] : mPreviews) {
        if (preview.descriptorSet != VK_NULL_HANDLE) {
            ImGui_ImplVulkan_RemoveTexture(preview.descriptorSet);
        }
    }
}

void CSpritesWidget::Render() {
    if (!ImGui::Begin("Loaded Sprites", &mVisible)) {
        ImGui::End();
        return;
    }

    ImGui::InputText("Filter", &mFilter);
    auto sprites = mSpriteManager.GetLoadedSprites();
    std::ranges::sort(sprites, [](const auto& left, const auto& right) {
        if (left.atlasName != right.atlasName) {
            return left.atlasName < right.atlasName;
        }
        return left.spriteName < right.spriteName;
    });

    std::unordered_map<uint64_t, VkImageView> currentViews;
    for (const auto& sprite : sprites) {
        const auto& texture = mTextureManager.GetTexture(sprite.rendering.id);
        if (texture.imageView != VK_NULL_HANDLE) {
            currentViews.emplace(sprite.rendering.id, texture.imageView);
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

    std::size_t visibleCount = 0;
    if (ImGui::BeginTable("SpriteList", 3,
                          ImGuiTableFlags_BordersInnerV |
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
                          ImVec2(0.0f, ImGui::GetContentRegionAvail().y))) {
        ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed,
                                84.0f);
        ImGui::TableSetupColumn("Sprite", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Atlas", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (const auto& sprite : sprites) {
            if (!mFilter.empty() &&
                sprite.spriteName.find(mFilter) == std::string::npos &&
                sprite.atlasName.find(mFilter) == std::string::npos) {
                continue;
            }
            ++visibleCount;
            ImGui::TableNextRow();
            ImGui::PushID(static_cast<int>(sprite.rendering.id));

            ImGui::TableSetColumnIndex(0);
            const auto preview = mPreviews.find(sprite.rendering.id);
            const auto& texture =
                mTextureManager.GetTexture(sprite.rendering.id);
            if (preview != mPreviews.end() &&
                preview->second.descriptorSet != VK_NULL_HANDLE &&
                sprite.rendering.pixelSize.x > 0.0f &&
                sprite.rendering.pixelSize.y > 0.0f) {
                const float scale =
                    std::min(72.0f / sprite.rendering.pixelSize.x,
                             72.0f / sprite.rendering.pixelSize.y);
                const ImVec2 size{sprite.rendering.pixelSize.x * scale,
                                  sprite.rendering.pixelSize.y * scale};
                const auto textureId = static_cast<ImTextureID>(
                    reinterpret_cast<uintptr_t>(preview->second.descriptorSet));
                ImGui::Image(
                    textureId, size,
                    ImVec2(sprite.rendering.uvMin.x, sprite.rendering.uvMin.y),
                    ImVec2(sprite.rendering.uvMax.x, sprite.rendering.uvMax.y));
            }

            ImGui::TableSetColumnIndex(1);
            ImGui::TextUnformatted(sprite.spriteName.c_str());
            ImGui::TextDisabled("%.0f x %.0f px", sprite.rendering.pixelSize.x,
                                sprite.rendering.pixelSize.y);

            ImGui::TableSetColumnIndex(2);
            ImGui::TextUnformatted(sprite.atlasName.c_str());
            ImGui::TextDisabled(
                "Texture #%llu  |  %u x %u",
                static_cast<unsigned long long>(sprite.rendering.id),
                texture.width, texture.height);
            ImGui::PopID();
        }
        ImGui::EndTable();
    }

    if (visibleCount == 0) {
        ImGui::TextDisabled(mFilter.empty() ? "No sprites are loaded."
                                            : "No sprites match the filter.");
    } else {
        ImGui::TextDisabled("%zu sprites", visibleCount);
    }
    ImGui::End();
}

const char* CSpritesWidget::GetName() const {
    return "Loaded Sprites";
}
} // namespace Debug
