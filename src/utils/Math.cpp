#include "utils/Math.h"

#include <cmath>

namespace Utils {
bool IsZero(const glm::vec2& vec, float epsilon) {
    return std::abs(vec.x) < epsilon && std::abs(vec.y) < epsilon;
}
} // namespace Utils