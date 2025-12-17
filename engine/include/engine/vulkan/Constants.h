#pragma once

namespace Vulkan {
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

constexpr int kVerticesBufferIndex = 0;
constexpr int kIndicesBufferIndex = 1;
constexpr int kInstanceBufferIndex = 2;
constexpr int kInstanceInfoBufferIndex = 3;
constexpr int kTextInstanceBufferIndex = 4;
constexpr int kUIVerticesBufferIndex = 5;
} // namespace Vulkan
