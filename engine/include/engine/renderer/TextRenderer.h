#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

struct SDL_GPUBuffer;
struct SDL_GPUDevice;
namespace Renderer {

// Per-character instance data
struct SCharacterInstance {
    glm::vec2 position{0.0f};
    glm::vec2 size{1.0f};
    glm::vec4 uvRect{0.0f};
    glm::vec4 color{1.0f};
};

class CTextRenderer {
public:
    explicit CTextRenderer(SDL_GPUDevice* device);
    ~CTextRenderer();

    void Initialize();

    SDL_GPUBuffer* GetQuadVertexBuffer() const;
    SDL_GPUBuffer* GetQuadIndexBuffer() const;

    SDL_GPUBuffer* CreateInstanceBuffer(size_t maxCharacters);

    void UpdateInstanceBuffer(SDL_GPUBuffer* buffer,
                              const std::vector<SCharacterInstance>& instances);

private:
    SDL_GPUDevice* mDevice;
    SDL_GPUBuffer* mQuadVertexBuffer = nullptr;
    SDL_GPUBuffer* mQuadIndexBuffer = nullptr;
};

} // namespace Renderer