#include "engine/vulkan/VertexBufferHandleWrapper.h"

#include "engine/vulkan/BufferHandle.h"

namespace Vulkan {

CVertexBufferHandleWrapper::CVertexBufferHandleWrapper(
    CBufferHandle* bufferHandle)
    : mBufferHandle(bufferHandle) {
}

std::optional<Utils::SBufferRange> CVertexBufferHandleWrapper::AddVertices(
    const std::vector<Renderer::SVertex>& vertices) {
    auto rangeOpt = mBufferHandle->Reserve(
        static_cast<int>(vertices.size() * sizeof(Renderer::SVertex)));
    if (!rangeOpt.has_value()) {
        return std::nullopt;
    }
    mBufferHandle->Fill(vertices.data(),
                        vertices.size() * sizeof(Renderer::SVertex), 0,
                        rangeOpt.value());
    mVertices.insert(mVertices.end(), vertices.begin(), vertices.end());
    return rangeOpt;
}

bool CVertexBufferHandleWrapper::UpdateVertex(
    const Renderer::SVertex& vertex, int index,
    const Utils::SBufferRange& range) {
    if (index < 0 || index * sizeof(Renderer::SVertex) >= range.size) {
        return false;
    }
    return mBufferHandle->Fill(&vertex, sizeof(Renderer::SVertex),
                               range.offset + index * sizeof(Renderer::SVertex),
                               const_cast<Utils::SBufferRange&>(range));
}

bool CVertexBufferHandleWrapper::UpdateVertices(
    const std::vector<Renderer::SVertex>& vertices, int startIndex,
    const Utils::SBufferRange& range) {
    if (startIndex < 0 || startIndex * sizeof(Renderer::SVertex) +
                                  vertices.size() * sizeof(Renderer::SVertex) >
                              range.size) {
        return false;
    }
    return mBufferHandle->Fill(
        vertices.data(),
        static_cast<int>(vertices.size() * sizeof(Renderer::SVertex)),
        range.offset + startIndex * sizeof(Renderer::SVertex), range);
}

const std::vector<Renderer::SVertex>&
CVertexBufferHandleWrapper::GetVertices() const {
    return mVertices;
}

} // namespace Vulkan