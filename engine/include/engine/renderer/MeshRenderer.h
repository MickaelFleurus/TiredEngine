#pragma once
#include <span>
#include <unordered_map>
#include <vector>

#include "engine/core/DataTypes.h"
#include "engine/core/GameObjectId.h"
#include "engine/renderer/IRenderer.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/utils/BufferMemoryBlocks.h"

namespace Core {
class CMesh;
} // namespace Core

namespace Renderer {
template <typename T>
class CRenderables;
} // namespace Renderer

namespace Vulkan {
template <typename T>
class CBufferHandleWrapper;
} // namespace Vulkan

namespace Renderer {
struct SPairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);
        // Combine hashes using XOR and bit shifting
        return h1 ^ (h2 << 1);
    }
};

class CMeshRenderer : public IRenderer {
public:
    CMeshRenderer(
        Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::SInstanceData>& instancesBuffer,
        Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
            indirectDrawBuffer);
    ~CMeshRenderer();

    void RegisterMesh(const Core::CMesh* mesh);
    void UnregisterMesh(const Core::CMesh* mesh);

    void Free() override;
    void Update() override;

    void UpdateInstances(
        Renderer::CRenderables<Renderer::SMeshRenderable>& renderables,
        const std::vector<Core::GameObjectId>& hidden);

    std::unordered_map<std::size_t, std::vector<Utils::SBufferIndexRange>>
    GetIndirectDrawRanges() const;

private:
    struct SMeshPipelineGroup {
        Utils::SBufferIndexRange instanceBufferRange;
        Utils::SBufferIndexRange indirectBufferRange;
        std::vector<Core::SInstanceData> instancesData;
        std::vector<Core::GameObjectId> gameObjectIds;
        std::optional<std::size_t>
        GetInstanceIndex(Core::GameObjectId id) const {
            auto it =
                std::find(gameObjectIds.cbegin(), gameObjectIds.cend(), id);
            if (it != gameObjectIds.cend()) {
                return std::distance(gameObjectIds.cbegin(), it);
            }
            return std::nullopt;
        }
    };

    struct SMeshDrawData {
        Utils::SBufferIndexRange verticesRange{};
        Utils::SBufferIndexRange indexesRange{};
    };

    Vulkan::CBufferHandleWrapper<Core::SVertex>& mVertexBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::IndexType>& mIndexesBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::SInstanceData>& mInstancesBuffer;
    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        mIndirectDrawBuffer;

    std::unordered_map<std::pair<std::size_t, std::size_t>, SMeshPipelineGroup,
                       SPairHash>
        mInstanceCache;

    std::unordered_map<std::size_t, SMeshDrawData> mMeshDrawRanges;
};
} // namespace Renderer