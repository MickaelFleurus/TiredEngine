#pragma once

#include <cstddef>

namespace Vulkan {
constexpr std::size_t kMaxTextures = 256;
constexpr std::size_t kMaxSSBO = 8192;
constexpr std::size_t kMaxUBO = 8192;
constexpr std::size_t kMaxMaterials = 256;
constexpr std::size_t kMaxSprites = 256;
constexpr std::size_t kMaxGlyphs = 256;
constexpr std::size_t kMaxDrawCount = 2048;
constexpr std::size_t kVertexAmountPerBuffer = 100000;
constexpr std::size_t k3DVertexAmountPerBuffer = 100000;
constexpr std::size_t k2DVertexAmountPerBuffer = 100000;
constexpr std::size_t kUIVertexAmountPerBuffer = 100000;
constexpr std::size_t kIndexAmountPerBuffer = 100000;
constexpr std::size_t kInstanceAmountPerBuffer = 8192;
constexpr std::size_t kInstanceInfoAmountPerBuffer = 8192;
constexpr std::size_t kTextInstanceAmountPerBuffer = 8192;
constexpr std::size_t kTextInstanceInfoAmountPerBuffer = 8192;

} // namespace Vulkan
