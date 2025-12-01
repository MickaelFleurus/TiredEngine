#include "engine/vulkan/IndexesBufferHandleWrapper.h"

#include "engine/vulkan/BufferHandle.h"

namespace Vulkan {

CIndexesBufferHandleWrapper::CIndexesBufferHandleWrapper(
    CBufferHandle* bufferHandle)
    : mBufferHandle(bufferHandle) {
}

std::optional<Utils::SBufferRange>
CIndexesBufferHandleWrapper::AddIndexes(const std::vector<uint32_t>& indexes) {
    auto rangeOpt = mBufferHandle->Reserve(
        static_cast<int>(indexes.size() * sizeof(uint32_t)));
    if (!rangeOpt.has_value()) {
        return std::nullopt;
    }
    mBufferHandle->Fill(indexes.data(), indexes.size() * sizeof(uint32_t), 0,
                        rangeOpt.value());
    return rangeOpt;
}
} // namespace Vulkan