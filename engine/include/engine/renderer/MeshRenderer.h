#pragma once
#include "engine/core/DataTypes.h"
#include "engine/core/GameObjectId.h"
#include "engine/renderer/IRenderer.h"
#include "engine/renderer/RendererUtils.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <span>
#include <unordered_map>
#include <vector>

namespace Core {
class CMesh;
class CCamera;
} // namespace Core

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

    void Free() override;
    void Update() override;

    void UpdateInstances(const std::vector<SRenderable>& renderables);

    std::unordered_map<std::size_t, std::vector<Utils::SBufferIndexRange>>
    GetIndirectDrawRanges() const;

private:
    struct SMeshPipelineGroup {
        Utils::SBufferIndexRange instanceBufferRange;
        Utils::SBufferIndexRange indirectBufferRange;
        std::vector<Core::SInstanceData> instancesData;
        std::vector<Core::GameObjectId> gameObjectIds;
    };

    struct SMeshDrawData {
        Utils::SBufferIndexRange verticesRange{};
        Utils::SBufferIndexRange indexesRange{};
        bool uploaded{false};
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