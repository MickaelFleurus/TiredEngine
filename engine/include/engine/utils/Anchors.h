#pragma once
#include <glm/vec2.hpp>

namespace Utils {
enum class EAnchors {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

constexpr inline glm::vec2 GetAnchorOffset(EAnchors anchor) {
    switch (anchor) {
    case EAnchors::TopLeft:
        return glm::vec2(0.0f, 1.0f);
    case EAnchors::TopCenter:
        return glm::vec2(0.5f, 1.0f);
    case EAnchors::TopRight:
        return glm::vec2(1.0f, 1.0f);
    case EAnchors::CenterLeft:
        return glm::vec2(0.0f, 0.5f);
    case EAnchors::Center:
        return glm::vec2(0.5f, 0.5f);
    case EAnchors::CenterRight:
        return glm::vec2(1.0f, 0.5f);
    case EAnchors::BottomLeft:
        return glm::vec2(0.0f, 0.0f);
    case EAnchors::BottomCenter:
        return glm::vec2(0.5f, 0.0f);
    case EAnchors::BottomRight:
        return glm::vec2(1.0f, 0.0f);
    }
}
} // namespace Utils