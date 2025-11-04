#pragma once

struct SDL_GPUBuffer;

namespace Renderer {
class IRenderingData {
public:
    virtual ~IRenderingData() = default;

protected:
    virtual void GenerateGeometry() = 0;

    SDL_GPUBuffer* mVerticesBuffer = nullptr;
    SDL_GPUBuffer* mIndicesBuffer = nullptr;
};
} // namespace Renderer