#pragma once

#include "engine/renderer/DataTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <optional>
#include <vector>

namespace Vulkan {
class CBufferHandle;
class CVertexBufferHandleWrapper {
public:
    CVertexBufferHandleWrapper(CBufferHandle* bufferHandle);

    std::optional<Utils::SBufferRange>
    AddVertices(const std::vector<Renderer::SVertex>& vertices);

    bool UpdateVertex(const Renderer::SVertex& vertex, int index,
                      const Utils::SBufferRange& range);
    bool UpdateVertices(const std::vector<Renderer::SVertex>& vertices,
                        int startIndex, const Utils::SBufferRange& range);

    const std::vector<Renderer::SVertex>& GetVertices() const;

private:
    CBufferHandle* mBufferHandle;
    std::vector<Renderer::SVertex> mVertices;
};
} // namespace Vulkan