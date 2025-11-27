#pragma once

#include "engine/renderer/DataTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <optional>
#include <vector>

namespace Renderer {
class CBufferHandle;
class CVertexBufferHandleWrapper {
public:
    CVertexBufferHandleWrapper(CBufferHandle* bufferHandle);

    std::optional<Utils::SBufferRange>
    AddVertices(const std::vector<SVertex>& vertices);

    bool UpdateVertex(const SVertex& vertex, int index,
                      const Utils::SBufferRange& range);
    bool UpdateVertices(const std::vector<SVertex>& vertices, int startIndex,
                        const Utils::SBufferRange& range);

    const std::vector<SVertex>& GetVertices() const;

private:
    CBufferHandle* mBufferHandle;
    std::vector<SVertex> mVertices;
};
} // namespace Renderer