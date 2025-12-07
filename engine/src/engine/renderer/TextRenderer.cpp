#include "engine/renderer/TextRenderer.h"

#include "engine/component/TextComponent.h"
#include "engine/core/DataTypes.h"
#include "engine/font/Font.h"
#include "engine/font/Police.h"
#include "engine/vulkan/BufferHandleWrapper.h"

#include <array>
#include <cstring>

namespace {
constexpr size_t kMaxCharacters = 500;

std::optional<Utils::SBufferIndexRange> CreateVertices(
    Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle) {
    std::vector<Core::SVertex> vertices;
    Core::SVertex topLeft{.position{0.0f, 0.0f, 0.0f}};
    Core::SVertex topRight{.position{1.0f, 0.0f, 0.0f}};
    Core::SVertex bottomRight{.position{1.0f, 1.0f, 0.0f}};
    Core::SVertex bottomLeft{.position{0.0f, 1.0f, 0.0f}};

    vertices.reserve(4);
    vertices.push_back(topLeft);
    vertices.push_back(topRight);
    vertices.push_back(bottomRight);
    vertices.push_back(bottomLeft);
    Utils::SBufferIndexRange range;
    if (vertexBufferHandle.PrepareData(range, vertices)) {
        return range;
    }
    return std::nullopt;
}

std::optional<Utils::SBufferIndexRange> CreateIndexes(
    Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
    const std::optional<Utils::SBufferIndexRange>& vertexBufferRange) {
    if (!vertexBufferRange.has_value()) {
        LOG_ERROR("Vertex buffer range is not valid!");
        return std::nullopt;
    }

    std::vector<uint32_t> indexes;
    indexes.reserve(6);
    indexes.push_back(vertexBufferRange->first + 0);
    indexes.push_back(vertexBufferRange->first + 1);
    indexes.push_back(vertexBufferRange->first + 2);

    indexes.push_back(vertexBufferRange->first + 2);
    indexes.push_back(vertexBufferRange->first + 1);
    indexes.push_back(vertexBufferRange->first + 3);

    Utils::SBufferIndexRange range;
    if (indexesBufferHandle.PrepareData(range, indexes)) {
        return range;
    }
    return std::nullopt;
}

} // namespace

namespace Renderer {

CTextRenderer::CTextRenderer(
    Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::STextInstanceData>& textInstanceBuffer,
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        indirectDrawBuffer)
    : mTextInstanceBuffer(textInstanceBuffer)
    , mIndirectDrawBuffer(indirectDrawBuffer)
    , mVertexBufferHandle(vertexBufferHandle)
    , mIndexesBufferHandle(indexesBufferHandle) {
}

void CTextRenderer::Free() {
    mIndexesRange.reset();
    mVerticesRange.reset();
}

void CTextRenderer::Prepare() {
    mVerticesRange = CreateVertices(mVertexBufferHandle);
    mIndexesRange = CreateIndexes(mIndexesBufferHandle, mVerticesRange);
}

std::unordered_map<Material::CAbstractMaterial*,
                   std::vector<Utils::SBufferIndexRange>>
CTextRenderer::GenerateInstances(const std::vector<SRenderable>&) {
    if (mNeedUpdate) {
        if (!mVerticesRange.has_value() || !mIndexesRange.has_value()) {
            LOG_ERROR("Vertex or Index buffer not initialized!");
            return {};
        }
        GenerateInstanceData();
        mNeedUpdate = false;
    }
    return {};
}

void CTextRenderer::Update() {
    // Implementation for updating text data goes here
}

void CTextRenderer::GenerateInstanceData() {

    std::unordered_map<Font::SFont, std::vector<Core::STextInstanceData>,
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

    std::vector<Core::STextInstanceData> allInstances;
    for (const auto& [font, instances] : instancesMap) {
        SIndirectDrawInfo info{};
        info.firstIndex = mIndexesRange->first;
        info.vertexOffset = mVerticesRange->first;
        info.firstInstance = static_cast<uint32_t>(mIndirectDrawInfos.size());
        info.instanceCount = static_cast<uint32_t>(instances.size());
        mIndirectDrawInfos.push_back(info);
        allInstances.insert(allInstances.end(), instances.begin(),
                            instances.end());
    }
    Utils::SBufferIndexRange textInstanceRange;
    if (!mTextInstanceBuffer.PrepareData(textInstanceRange, allInstances)) {
        LOG_ERROR("Failed to prepare text instance buffer!");
        return;
    }

    std::vector<Core::SIndirectDrawCommand> indirectCommands;
    for (const auto& r : mIndirectDrawInfos) {
        Core::SIndirectDrawCommand cmd{};
        cmd.indexCount = 6;
        cmd.instanceCount = r.instanceCount;
        cmd.firstIndex = r.firstIndex;
        cmd.vertexOffset = r.vertexOffset;
        cmd.firstInstance = r.firstInstance;
        indirectCommands.push_back(cmd);
    }

    Utils::SBufferIndexRange range;
    if (!mIndirectDrawBuffer.PrepareData(range, indirectCommands)) {
        LOG_ERROR("Failed to prepare indirect draw buffer!");
        return;
    }
    LOG_INFO("Generated {} text instances for rendering.", allInstances.size());
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