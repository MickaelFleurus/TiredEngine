#include "engine/renderer/TextRenderer.h"

#include <array>
#include <cstring>

#include "engine/component/TextUIComponent.h"
#include "engine/core/DataTypes.h"
#include "engine/font/Font.h"
#include "engine/font/Police.h"
#include "engine/renderer/Renderables.h"
#include "engine/vulkan/BufferHandleWrapper.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

namespace {
constexpr size_t kMaxCharacters = 500;

std::optional<Utils::SBufferIndexRange> CreateVertices(
    Vulkan::CBufferHandleWrapper<Core::SUIVertex>& vertexBufferHandle) {
    std::vector<Core::SUIVertex> vertices;
    Core::SUIVertex topLeft{.position{0.0f, 0.0f}, .texCoord{0.0f, 0.0f}};
    Core::SUIVertex topRight{.position{1.0f, 0.0f}, .texCoord{1.0f, 0.0f}};
    Core::SUIVertex bottomRight{.position{1.0f, 1.0f}, .texCoord{1.0f, 1.0f}};
    Core::SUIVertex bottomLeft{.position{0.0f, 1.0f}, .texCoord{0.0f, 1.0f}};

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
    Vulkan::CBufferHandleWrapper<Core::TextIndexType>& indexesBufferHandle,
    const std::optional<Utils::SBufferIndexRange>& vertexBufferRange) {
    if (!vertexBufferRange.has_value()) {
        LOG_ERROR("Vertex buffer range is not valid!");
        return std::nullopt;
    }

    std::vector<Core::TextIndexType> indexes;
    indexes.reserve(6);
    indexes.push_back(vertexBufferRange->first + 0);
    indexes.push_back(vertexBufferRange->first + 1);
    indexes.push_back(vertexBufferRange->first + 2);

    indexes.push_back(vertexBufferRange->first + 2);
    indexes.push_back(vertexBufferRange->first + 3);
    indexes.push_back(vertexBufferRange->first + 0);

    Utils::SBufferIndexRange range;
    if (indexesBufferHandle.PrepareData(range, indexes)) {
        return range;
    }
    return std::nullopt;
}

std::optional<std::size_t>
GetInstanceIndex(const std::vector<Core::GameObjectId>& gameObjectIds,
                 Core::GameObjectId id) {
    auto it = std::find(gameObjectIds.cbegin(), gameObjectIds.cend(), id);
    if (it != gameObjectIds.cend()) {
        return std::distance(gameObjectIds.cbegin(), it);
    }
    return std::nullopt;
}

} // namespace

namespace Renderer {

CTextRenderer::CTextRenderer(
    Vulkan::CBufferHandleWrapper<Core::SUIVertex>& vertexBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::TextIndexType>& indexesBufferHandle,
    Vulkan::CBufferHandleWrapper<Core::SUIInstanceData>& textInstanceBuffer,
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        indirectDrawBuffer)
    : mTextInstanceBuffer(textInstanceBuffer)
    , mIndirectDrawBuffer(indirectDrawBuffer)
    , mVertexBufferHandle(vertexBufferHandle)
    , mIndexesBufferHandle(indexesBufferHandle) {
}

void CTextRenderer::Prepare() {
    mVerticesRange = CreateVertices(mVertexBufferHandle);
    mIndexesRange = CreateIndexes(mIndexesBufferHandle, mVerticesRange);
}

void CTextRenderer::Update() {
    // Implementation for updating text data goes here
}

void CTextRenderer::UpdateInstances(
    Renderer::CRenderables<Renderer::STextRenderable>& renderables,
    const std::vector<Core::GameObjectId>& hidden) {
    std::vector<std::size_t> requireInstanceUpdate;
    std::vector<std::size_t> requireIndirectUpdate;

    for (auto& renderable : renderables.GetUpdateRenderables()) {
        auto instanceIndex = GetInstanceIndex(mGameObjectIds, renderable.id);
        if (instanceIndex.has_value()) {
            requireInstanceUpdate.push_back(*instanceIndex);
            mInstancesData[*instanceIndex].swap(renderable.instancesData);
        } else {
            LOG_WARNING("Renderable with id {} not found in instance cache.",
                        renderable.id);
        }
    }

    for (const auto& id : hidden) {
        if (auto instanceIndex = GetInstanceIndex(mGameObjectIds, id);
            instanceIndex.has_value()) {
            requireIndirectUpdate.push_back(*instanceIndex);
            mInstancesData.erase(mInstancesData.begin() + *instanceIndex);
            mGameObjectIds.erase(mGameObjectIds.begin() + *instanceIndex);
        }
    }

    for (auto& renderable : renderables.GetReorderRenderables()) {
        if (auto instanceIndex =
                GetInstanceIndex(mGameObjectIds, renderable.id);
            instanceIndex.has_value()) {
            requireIndirectUpdate.push_back(*instanceIndex);
            requireInstanceUpdate.push_back(*instanceIndex);
            mInstancesData[*instanceIndex].swap(renderable.instancesData);
        } else {
            requireIndirectUpdate.push_back(mInstancesData.size());
            requireInstanceUpdate.push_back(mInstancesData.size());
            mInstancesData.push_back(renderable.instancesData);
            mGameObjectIds.push_back(renderable.id);
            mTextInstanceBufferRanges.push_back(Utils::SBufferIndexRange{0, 0});
            mIndirectDrawBufferRanges.push_back(Utils::SBufferIndexRange{0, 0});
        }
    }

    for (const auto& key : requireInstanceUpdate) {
        if (!mTextInstanceBuffer.PrepareData(mTextInstanceBufferRanges[key],
                                             mInstancesData[key])) {
            LOG_WARNING("Failed to prepare instance data!");
        }
    }

    for (const auto& key : requireIndirectUpdate) {
        Core::SIndirectDrawCommand cmd{};
        cmd.indexCount = static_cast<uint32_t>(mIndexesRange->count);
        cmd.instanceCount = static_cast<uint32_t>(mInstancesData[key].size());
        cmd.firstIndex = static_cast<uint32_t>(mIndexesRange->first);
        cmd.vertexOffset = static_cast<int32_t>(mVerticesRange->first);
        cmd.firstInstance =
            static_cast<uint32_t>(mTextInstanceBufferRanges[key].first);
        if (!mIndirectDrawBuffer.PrepareData(mIndirectDrawBufferRanges[key],
                                             cmd)) {
            LOG_WARNING("Failed to prepare indirect draw command!");
        }
    }
}

const std::vector<Utils::SBufferIndexRange>&
CTextRenderer::GetIndirectDrawRange() const {
    return mIndirectDrawBufferRanges;
}

} // namespace Renderer