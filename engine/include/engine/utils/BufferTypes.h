#pragma once
#include <cstdint>

namespace Utils {

struct SBufferRange {
    uint64_t offset{};
    uint64_t size{};
    constexpr bool IsInitialized() const {
        return size > 0;
    }
};

struct SBufferIndexRange {
    uint64_t first{};
    uint64_t count{};
    constexpr bool IsInitialized() const {
        return count > 0;
    }
};

} // namespace Utils
