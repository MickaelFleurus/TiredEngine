#pragma once

#include "engine/renderer/DataTypes.h"
#include "engine/utils/BufferMemoryBlocks.h"

#include <optional>
#include <vector>

namespace Vulkan {
class CBufferHandle;
class CIndexesBufferHandleWrapper {
public:
    CIndexesBufferHandleWrapper(CBufferHandle* bufferHandle);

    std::optional<Utils::SBufferRange>
    AddIndexes(const std::vector<uint32_t>& indexes);

private:
    CBufferHandle* mBufferHandle;
};
} // namespace Vulkan