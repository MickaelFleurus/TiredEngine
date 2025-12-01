#include "engine/renderer/TextRenderer.h"
#include "engine/renderer/DataTypes.h"

#include "engine/component/TextComponent.h"
#include "engine/font/Font.h"
#include "engine/font/Police.h"
#include "engine/vulkan/BufferHandle.h"
#include "engine/vulkan/IndexesBufferHandleWrapper.h"
#include "engine/vulkan/VertexBufferHandleWrapper.h"

#include <array>
#include <cstring>

namespace {
constexpr size_t kMaxCharacters = 500;

Utils::SBufferRange
CreateVertices(Vulkan::CVertexBufferHandleWrapper& vertexBufferHandle) {
    std::vector<Renderer::SVertex> vertices;
    Renderer::SVertex topLeft{.position{0.0f, 0.0f, 0.0f}};
    Renderer::SVertex topRight{.position{1.0f, 0.0f, 0.0f}};
    Renderer::SVertex bottomRight{.position{1.0f, 1.0f, 0.0f}};
    Renderer::SVertex bottomLeft{.position{0.0f, 1.0f, 0.0f}};
    vertices.reserve(4);
    vertices.push_back(topLeft);
    vertices.push_back(topRight);
    vertices.push_back(bottomRight);
    vertices.push_back(bottomLeft);

    return vertexBufferHandle.AddVertices(vertices).value_or(
        Utils::SBufferRange{0, 0});
}

Utils::SBufferRange
CreateIndexes(Vulkan::CIndexesBufferHandleWrapper& indexesBufferHandle,
              const Utils::SBufferRange& vertexBufferRange) {

    std::vector<uint32_t> indexes;
    indexes.reserve(6);
    const int index = vertexBufferRange.offset / sizeof(Renderer::SVertex);
    indexes.push_back(index + 0);
    indexes.push_back(index + 1);
    indexes.push_back(index + 2);

    indexes.push_back(index + 2);
    indexes.push_back(index + 1);
    indexes.push_back(index + 3);
    return indexesBufferHandle.AddIndexes(indexes).value_or(
        Utils::SBufferRange{0, 0});
}

} // namespace

namespace Renderer {

CTextRenderer::CTextRenderer(
    Vulkan::CVertexBufferHandleWrapper& vertexBufferHandle,
    Vulkan::CIndexesBufferHandleWrapper& indexesBufferHandle,
    Vulkan::CBufferHandle& textInstanceBuffer, Vulkan::CBufferHandle& textInstanceInfoBuffer)
    : mTextInstanceBuffer(textInstanceBuffer)
    , mTextInstanceBufferRange(
          textInstanceBuffer
              .Reserve(static_cast<int>(kMaxCharacters * sizeof(SInstanceData)))
              .value_or(Utils::SBufferRange{0, 0}))
    , mTextInstanceInfoBuffer(textInstanceInfoBuffer)
    , mVerticesRange(CreateVertices(vertexBufferHandle))
    , mIndexesRange(CreateIndexes(indexesBufferHandle, mVerticesRange)) {
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
        vkCmdDrawIndexedIndirect(
            commandBuffer, mTextInstanceInfoBuffer.GetBuffer(),
            ++offset * sizeof(VkDrawIndexedIndirectCommand), 1,
            sizeof(VkDrawIndexedIndirectCommand));
    }
}

void CTextRenderer::GenerateInstanceData() {
    std::unordered_map<Font::SFont, std::vector<Renderer::STextInstanceData>,
                       Font::SFontHash>
        instancesMap;

    for (auto* textComponent : mRegisteredTextComponents) {
        const auto& instances = textComponent->GetInstances();
        if (instances.empty()) {
            continue;
        }
        for (char c : textComponent->getText()) {
            Font::SFont fontKey{textComponent->getPolice()->GetName(), c};
            instancesMap[fontKey].insert(instancesMap[fontKey].end(),
                                         instances.begin(), instances.end());
        }
    }

    std::vector<STextInstanceData> allInstances;
    auto firstIndex = mIndexesRange.offset / sizeof(uint32_t);
    auto firstVertice = mVerticesRange.offset / sizeof(Renderer::SVertex);
    for (const auto& [font, instances] : instancesMap) {

        SInstanceInfo info{};
        info.firstIndex = firstIndex;
        info.vertexOffset = firstVertice;
        info.firstInstance = static_cast<uint32_t>(mInstanceInfos.size());
        info.instanceCount = static_cast<uint32_t>(instances.size());
        mInstanceInfos.push_back(info);
        allInstances.insert(allInstances.end(), instances.begin(),
                            instances.end());
    }

    mTextInstanceBuffer.Fill(
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

    mTextInstanceInfoBufferRange =
        mTextInstanceInfoBuffer
            .Reserve(static_cast<int>(indirectCommands.size() *
                                      sizeof(VkDrawIndexedIndirectCommand)))
            .value_or(Utils::SBufferRange{0, 0});
    mTextInstanceInfoBuffer.Fill(
        indirectCommands.data(),
        static_cast<int>(indirectCommands.size() *
                         sizeof(VkDrawIndexedIndirectCommand)),
        0, mTextInstanceInfoBufferRange);
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