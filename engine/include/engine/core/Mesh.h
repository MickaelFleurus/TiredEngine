#pragma once

#include <cstdint>

#include <engine/utils/StringId.h>
#include <glm/mat4x4.hpp>

namespace Core {

struct SMesh {
    const CStringId id;
    const uint64_t meshInfoId;
    const glm::mat4 localTransform = glm::mat4(1.0f);
    SMesh* parent = nullptr;
};
} // namespace Core
