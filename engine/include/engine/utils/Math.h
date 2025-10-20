#pragma once

#include <glm/vec2.hpp>

namespace Utils {
bool IsZero(const glm::vec2& vec, float epsilon = 1e-6f);
} // namespace Utils