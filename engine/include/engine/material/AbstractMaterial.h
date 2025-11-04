#pragma once
#include "engine/material/MaterialTypes.h"

#include <glm/vec4.hpp>

struct SDL_GPURenderPass;
struct SDL_GPUGraphicsPipeline;
struct SDL_GPUTexture;

namespace Material {

class AbstractMaterial {
public:
    virtual ~AbstractMaterial() = default;

    EMaterialType GetType() const;
    SDL_GPUGraphicsPipeline* GetPipeline() const;

    void SetColor(const glm::vec4& color);
    void SetTexture(SDL_GPUTexture* texture);

    const glm::vec4& GetColor() const;
    SDL_GPUTexture* GetTexture() const;

    virtual void Bind(SDL_GPURenderPass* renderPass);

protected:
    explicit AbstractMaterial(EMaterialType type,
                              SDL_GPUGraphicsPipeline* pipeline);
    AbstractMaterial(const AbstractMaterial& other);
    AbstractMaterial& operator=(const AbstractMaterial& other);
    AbstractMaterial(AbstractMaterial&& other) noexcept;
    AbstractMaterial& operator=(AbstractMaterial&& other) noexcept;

    EMaterialType mType = EMaterialType::Unlit;
    SDL_GPUGraphicsPipeline* mPipeline = nullptr;
    glm::vec4 mColor = glm::vec4(1.0f);
    SDL_GPUTexture* mTexture = nullptr;
};

} // namespace Material