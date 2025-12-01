#pragma once
#include <cstddef>

namespace Utils {

struct SBufferRange {
    std::size_t offset;
    std::size_t size;
};

struct SBufferIndexRange {
    std::size_t first;
    std::size_t count;
};

} // namespace Utils