#pragma once

#include <vector>

#include "engine/core/DataTypes.h"
#include "engine/renderer/IRenderer.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/utils/BufferMemoryBlocks.h"

namespace Vulkan {
template <typename T>
class CBufferHandleWrapper;
} // namespace Vulkan

namespace Renderer {

template <typename T>
class CRenderables;

class CTextRenderer : public IRenderer {
public:
    explicit CTextRenderer(
        Vulkan::CBufferHandleWrapper<Core::SUIVertex>& vertexBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::SUIInstanceData>& instanceBuffer,
        Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
            indirectDrawBuffer);
    void Free() override {
    }
    void Prepare();
    void Update() override;

    void UpdateInstances(CRenderables<STextRenderable>& renderables,
                         const std::vector<Core::GameObjectId>& hidden);

    const std::vector<Utils::SBufferIndexRange>& GetIndirectDrawRange() const;

private:
    Vulkan::CBufferHandleWrapper<Core::SUIVertex>& mVertexBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::IndexType>& mIndexesBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::SUIInstanceData>& mTextInstanceBuffer;
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        mIndirectDrawBuffer;

    std::vector<std::vector<Core::SUIInstanceData>> mInstancesData;
    std::vector<Core::GameObjectId> mGameObjectIds;
    std::vector<Utils::SBufferIndexRange> mTextInstanceBufferRanges;
    std::vector<Utils::SBufferIndexRange> mIndirectDrawBufferRanges;

    std::optional<Utils::SBufferIndexRange> mVerticesRange{std::nullopt};
    std::optional<Utils::SBufferIndexRange> mIndexesRange{std::nullopt};
};

} // namespace Renderer
