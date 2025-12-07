#pragma once

#include <unordered_map>
#include <vector>

namespace Utils {
struct SBufferIndexRange;
}

namespace Material {
class CAbstractMaterial;
} // namespace Material

namespace Renderer {
struct SRenderable;
class IRenderer {
public:
    virtual ~IRenderer() = default;

    virtual void Free() = 0;
    virtual void Update() = 0;
};
} // namespace Renderer