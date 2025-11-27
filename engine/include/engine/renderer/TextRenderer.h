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

class CTextRenderer {
public:
    struct SInstanceInfo {
        uint32_t firstInstance;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t vertexOffset;
    };
    explicit CTextRenderer(CBufferHandle& instanceBuffer,
                           CBufferHandle& instanceInfoBuffer);

    void Update();
    void DrawTexts(VkCommandBuffer commandBuffer);

    void SetNeedUpdate();

    void RegisterTextComponent(Component::CTextComponent* textComponent);
    void UnregisterTextComponent(Component::CTextComponent* textComponent);

    // SBufferRange ReserveBufferRange(size_t maxCharacters);

    // void UpdateInstanceBuffer(SBufferRange range,
    //                           const std::vector<SInstanceData>& instances);

private:
    void GenerateInstanceData();
    CBufferHandle& mInstanceBuffer;
    Utils::SBufferRange mTextInstanceBufferRange;

    CBufferHandle& mInstanceInfoBuffer;
    Utils::SBufferRange mInstanceInfoBufferRange;
    bool mNeedUpdate = true;

    std::vector<SInstanceInfo> mInstanceInfos;
    std::vector<Component::CTextComponent*> mRegisteredTextComponents;
};

} // namespace Renderer