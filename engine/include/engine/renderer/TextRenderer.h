#pragma once

#include "engine/renderer/BufferHandle.h"
#include "engine/renderer/DataTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <string>
#include <unordered_map>
#include <vector>

namespace Component {
class CTextComponent;
}

namespace Renderer {

class CVertexBufferHandleWrapper;
class CIndexesBufferHandleWrapper;

class CTextRenderer {
public:
    struct SInstanceInfo {
        uint32_t firstInstance;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t vertexOffset;
    };
    explicit CTextRenderer(
        Renderer::CVertexBufferHandleWrapper& vertexBufferHandle,
        Renderer::CIndexesBufferHandleWrapper& mIndexesBufferHandle,
        CBufferHandle& instanceBuffer, CBufferHandle& instanceInfoBuffer);

    void Update();
    void DrawTexts(VkCommandBuffer commandBuffer);

    void SetNeedUpdate();

    void RegisterTextComponent(Component::CTextComponent* textComponent);
    void UnregisterTextComponent(Component::CTextComponent* textComponent);

private:
    void GenerateInstanceData();
    CBufferHandle& mTextInstanceBuffer;
    Utils::SBufferRange mTextInstanceBufferRange;

    CBufferHandle& mTextInstanceInfoBuffer;
    Utils::SBufferRange mTextInstanceInfoBufferRange;
    bool mNeedUpdate = true;

    std::vector<SInstanceInfo> mInstanceInfos;
    std::vector<Component::CTextComponent*> mRegisteredTextComponents;

    Utils::SBufferRange mVerticesRange;
    Utils::SBufferRange mIndexesRange;
};

} // namespace Renderer