#pragma once

#include <cstdint>
#include <limits>

namespace Core {
struct SEntity {
    uint32_t id;
    uint32_t generation;
};

constexpr SEntity kNullEntity{std::numeric_limits<uint32_t>().max(),
                              std::numeric_limits<uint32_t>().max()};
} // namespace Core
