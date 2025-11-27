#include "engine/renderer/TextRenderer.h"
#include "engine/renderer/DataTypes.h"

#include "engine/component/TextComponent.h"
#include "engine/font/Font.h"
#include "engine/font/Police.h"

#include <array>
#include <cstring>

namespace {
constexpr size_t kMaxCharacters = 500;

} // namespace

namespace Renderer {

CTextRenderer::CTextRenderer(CBufferHandle& instanceBuffer,
                             CBufferHandle& instanceInfoBuffer)
    : mInstanceBuffer(instanceBuffer)
    , mTextInstanceBufferRange(
          instanceBuffer
              .Reserve(static_cast<int>(kMaxCharacters * sizeof(SInstanceData)))
              .value())
    , mInstanceInfoBuffer(instanceInfoBuffer) {
}

void CTextRenderer::Update() {
    if (mNeedUpdate) {
        GenerateInstanceData();
        mNeedUpdate = false;
    }
}

void CTextRenderer::DrawTexts(VkCommandBuffer commandBuffer) {
    int offset = 0;
    for (const auto& r : mInstanceInfos) {
        vkCmdDrawIndexedIndirect(commandBuffer, mInstanceInfoBuffer.GetBuffer(),
                                 ++offset *
                                     sizeof(VkDrawIndexedIndirectCommand),
                                 1, sizeof(VkDrawIndexedIndirectCommand));
    }
}

void CTextRenderer::GenerateInstanceData() {
    std::unordered_map<Font::SFont, std::vector<Renderer::SInstanceData>,
                       Font::SFontHash>
        instancesMap;
    std::unordered_map<Font::SFont, Font::CPolice*, Font::SFontHash> polices;
    for (auto* textComponent : mRegisteredTextComponents) {
        const auto& instances = textComponent->GetInstances();
        if (instances.empty()) {
            continue;
        }
        for (char c : textComponent->getText()) {
            Font::SFont fontKey{textComponent->getPolice()->GetName(), c};
            instancesMap[fontKey].insert(instancesMap[fontKey].end(),
                                         instances.begin(), instances.end());
            polices[fontKey] = textComponent->getPolice();
        }
    }

    std::vector<SInstanceData> allInstances;
    for (const auto& [font, instances] : instancesMap) {

        SInstanceInfo info{};
        info.firstIndex = polices[font]->GetIndexOffset(font.character);
        info.vertexOffset = polices[font]->GetVertexOffset(font.character);
        info.firstInstance = static_cast<uint32_t>(mInstanceInfos.size());
        info.instanceCount = static_cast<uint32_t>(instances.size());
        mInstanceInfos.push_back(info);
        allInstances.insert(allInstances.end(), instances.begin(),
                            instances.end());
    }

    mInstanceBuffer.Fill(
        allInstances.data(),
        static_cast<int>(allInstances.size() * sizeof(SInstanceData)), 0,
        mTextInstanceBufferRange);

    std::vector<VkDrawIndexedIndirectCommand> indirectCommands;
    for (const auto& r : mInstanceInfos) {
        VkDrawIndexedIndirectCommand cmd{};
        cmd.indexCount = 6;
        cmd.instanceCount = r.instanceCount;
        cmd.firstIndex = r.firstIndex;
        cmd.vertexOffset = r.vertexOffset;
        cmd.firstInstance = r.firstInstance;
        indirectCommands.push_back(cmd);
    }

    mInstanceInfoBufferRange =
        mInstanceInfoBuffer
            .Reserve(static_cast<int>(indirectCommands.size() *
                                      sizeof(VkDrawIndexedIndirectCommand)))
            .value();
    mInstanceInfoBuffer.Fill(
        indirectCommands.data(),
        static_cast<int>(indirectCommands.size() *
                         sizeof(VkDrawIndexedIndirectCommand)),
        0, mInstanceInfoBufferRange);
}

void CTextRenderer::SetNeedUpdate() {
    mNeedUpdate = true;
}

void CTextRenderer::RegisterTextComponent(
    Component::CTextComponent* textComponent) {
    mRegisteredTextComponents.push_back(textComponent);
    mNeedUpdate = true;
}

void CTextRenderer::UnregisterTextComponent(
    Component::CTextComponent* textComponent) {
    auto it = std::find(mRegisteredTextComponents.begin(),
                        mRegisteredTextComponents.end(), textComponent);
    if (it != mRegisteredTextComponents.end()) {
        mRegisteredTextComponents.erase(it);
        mNeedUpdate = true;
    }
}

} // namespace Renderer