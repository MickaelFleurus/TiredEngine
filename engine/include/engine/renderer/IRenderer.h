#pragma once

namespace Renderer {
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Free() = 0;
    virtual void Update() = 0;
};
} // namespace Renderer
