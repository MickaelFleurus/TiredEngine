#pragma once

#include "engine/core/DataTypes.h"
#include "engine/renderer/IRenderer.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan.h>

namespace Component {
class CTextComponent;
}

namespace Vulkan {
template <typename T>
class CBufferHandleWrapper;
} // namespace Vulkan

namespace Renderer {

class CTextRenderer : public IRenderer {
public:
    struct SIndirectDrawInfo {
        uint32_t firstInstance;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t vertexOffset;
    };
    explicit CTextRenderer(
        Vulkan::CBufferHandleWrapper<Core::SVertex>& vertexBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::IndexType>& indexesBufferHandle,
        Vulkan::CBufferHandleWrapper<Core::STextInstanceData>& instanceBuffer,
        Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
            indirectDrawBuffer);
    void Free() override;
    void Prepare() override;
    void Update() override;
    // void DrawTextsIndirect(VkCommandBuffer commandBuffer);
    //  void DrawTextsDirect(VkCommandBuffer commandBuffer);

    void SetNeedUpdate();
    std::unordered_map<Material::CAbstractMaterial*,
                       std::vector<Utils::SBufferIndexRange>>
    RebuildInstances(const std::vector<SRenderable>& renderables) override;

    void RegisterTextComponent(Component::CTextComponent* textComponent);
    void UnregisterTextComponent(Component::CTextComponent* textComponent);

private:
    void GenerateInstanceData();
    Vulkan::CBufferHandleWrapper<Core::STextInstanceData>& mTextInstanceBuffer;
    std::optional<Utils::SBufferIndexRange> mTextInstanceBufferRange{
        std::nullopt};

    Vulkan::CBufferHandleWrapper<Core::SIndirectDrawCommand>&
        mIndirectDrawBuffer;
    std::optional<Utils::SBufferIndexRange> mIndirectDrawBufferRange{
        std::nullopt};

    Vulkan::CBufferHandleWrapper<Core::SVertex>& mVertexBufferHandle;
    Vulkan::CBufferHandleWrapper<Core::IndexType>& mIndexesBufferHandle;
    std::optional<Utils::SBufferIndexRange> mVerticesRange{std::nullopt};
    std::optional<Utils::SBufferIndexRange> mIndexesRange{std::nullopt};

    bool mNeedUpdate = true;

    std::vector<SIndirectDrawInfo> mIndirectDrawInfos;
    std::vector<Component::CTextComponent*> mRegisteredTextComponents;
};

} // namespace Renderer