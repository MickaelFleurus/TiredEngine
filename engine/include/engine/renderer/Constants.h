#pragma once

namespace Renderer {
constexpr size_t kMaxTextures = 256;
constexpr size_t kMaxSSBO = 8192;
constexpr size_t kMaxUBO = 8192;
constexpr size_t kMaxMaterials = 256;
constexpr size_t kVertexAmountPerBuffer = 100000;
constexpr size_t kIndexAmountPerBuffer = 100000;
constexpr size_t kInstanceAmountPerBuffer = 8192;
constexpr size_t kInstanceInfoAmountPerBuffer = 8192;
constexpr size_t kTextInstanceAmountPerBuffer = 8192;
constexpr size_t kTextInstanceInfoAmountPerBuffer = 8192;

constexpr size_t kTextureBinding = 0;
constexpr size_t kInstanceBufferBinding = 1;
} // namespace Renderer