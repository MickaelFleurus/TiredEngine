#include "engine/renderer/VertexBufferHandleWrapper.h"

#include "engine/renderer/BufferHandle.h"

namespace Renderer {

CVertexBufferHandleWrapper::CVertexBufferHandleWrapper(
    CBufferHandle* bufferHandle)
    : mBufferHandle(bufferHandle) {
}

std::optional<Utils::SBufferRange>
CVertexBufferHandleWrapper::AddVertices(const std::vector<SVertex>& vertices) {
    auto rangeOpt = mBufferHandle->Reserve(
        static_cast<int>(vertices.size() * sizeof(SVertex)));
    if (!rangeOpt.has_value()) {
        return std::nullopt;
    }
    mBufferHandle->Fill(vertices.data(), vertices.size() * sizeof(SVertex), 0,
                        rangeOpt.value());
    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    return rangeOpt;
}

bool CVertexBufferHandleWrapper::UpdateVertex(
    const SVertex& vertex, int index, const Utils::SBufferRange& range) {
    if (index < 0 || index * sizeof(SVertex) >= range.size) {
        return false;
    }
    return mBufferHandle->Fill(&vertex, sizeof(SVertex),
                               range.offset + index * sizeof(SVertex),
                               const_cast<Utils::SBufferRange&>(range));
}

bool CVertexBufferHandleWrapper::UpdateVertices(
    const std::vector<SVertex>& vertices, int startIndex,
    const Utils::SBufferRange& range) {
    if (startIndex < 0 ||
        startIndex * sizeof(SVertex) + vertices.size() * sizeof(SVertex) >
            range.size) {
        return false;
    }
    return mBufferHandle->Fill(
        vertices.data(), static_cast<int>(vertices.size() * sizeof(SVertex)),
        range.offset + startIndex * sizeof(SVertex), range);
}

const std::vector<SVertex>& CVertexBufferHandleWrapper::GetVertices() const {
    return mVertices;
}

} // namespace Renderer