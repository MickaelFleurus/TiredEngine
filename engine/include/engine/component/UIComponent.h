#pragma once
#include <cstddef>
#include <string>

#include <glm/vec4.hpp>

namespace Component {
struct SText {
    std::string text;
    std::size_t policeId;
    float size;
    glm::vec4 color{1.0f};
};
} // namespace Component
