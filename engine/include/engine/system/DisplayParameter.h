#pragma once
#include <optional>

namespace System {
struct SDisplayParameter {
    int width = 800;
    int height = 600;
    bool fullscreen = false;
    bool vSync = false;
    std::optional<int> fpsLimit;
};
} // namespace System