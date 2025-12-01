#pragma once

#include "engine/renderer/DataTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"
#include <vulkan/vulkan.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace Component {
class CTextComponent;
}

namespace Vulkan {
class CBufferHandle;
class CVertexBufferHandleWrapper;
class CIndexesBufferHandleWrapper;
} // namespace Vulkan

namespace Renderer {

class CTextRenderer {
public:
    struct SInstanceInfo {
        uint32_t firstInstance;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t vertexOffset;
    };
    explicit CTextRenderer(
        Vulkan::CVertexBufferHandleWrapper& vertexBufferHandle,
        Vulkan::CIndexesBufferHandleWrapper& mIndexesBufferHandle,
        Vulkan::CBufferHandle& instanceBuffer,
        Vulkan::CBufferHandle& instanceInfoBuffer);

    void Update();
    void DrawTexts(VkCommandBuffer commandBuffer);

    void SetNeedUpdate();

    void RegisterTextComponent(Component::CTextComponent* textComponent);
    void UnregisterTextComponent(Component::CTextComponent* textComponent);

private:
    void GenerateInstanceData();
    Vulkan::CBufferHandle& mTextInstanceBuffer;
    Utils::SBufferRange mTextInstanceBufferRange;

    Vulkan::CBufferHandle& mTextInstanceInfoBuffer;
    Utils::SBufferRange mTextInstanceInfoBufferRange;
    bool mNeedUpdate = true;

    std::vector<SInstanceInfo> mInstanceInfos;
    std::vector<Component::CTextComponent*> mRegisteredTextComponents;

    Utils::SBufferRange mVerticesRange;
    Utils::SBufferRange mIndexesRange;
};

} // namespace Renderer