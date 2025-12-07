#pragma once
#include <cstdint>

namespace Utils {

struct SBufferRange {
    uint64_t offset{};
    uint64_t size{};
};

struct SBufferIndexRange {
    uint64_t first{};
    uint64_t count{};
};

} // namespace Utils