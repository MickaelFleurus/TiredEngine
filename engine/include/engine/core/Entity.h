#pragma once

#include <cstdint>
#include <limits>

#include "engine/utils/StringId.h"

namespace Core {
struct SEntity {
    CStringId name;
    uint32_t id;
    uint32_t generation;
};

constexpr SEntity kNullEntity{CStringId{},
                              std::numeric_limits<uint32_t>().max(),
                              std::numeric_limits<uint32_t>().max()};
} // namespace Core
