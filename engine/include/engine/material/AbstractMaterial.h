#pragma once
#include "engine/material/MaterialTypes.h"

#include <glm/vec4.hpp>
#include <vulkan/vulkan.h>

struct SDL_GPUTexture;

namespace Material {

class AbstractMaterial {
public:
    virtual ~AbstractMaterial() = default;

    EMaterialType GetType() const;
    VkPipeline GetPipeline() const;

    void SetColor(const glm::vec4& color);
    void SetTexture(SDL_GPUTexture* texture);

    const glm::vec4& GetColor() const;
    SDL_GPUTexture* GetTexture() const;

    virtual void Bind(VkRenderPass renderPass);

protected:
    explicit AbstractMaterial(EMaterialType type, VkPipeline pipeline);
    AbstractMaterial(const AbstractMaterial& other);
    AbstractMaterial& operator=(const AbstractMaterial& other);
    AbstractMaterial(AbstractMaterial&& other) noexcept;
    AbstractMaterial& operator=(AbstractMaterial&& other) noexcept;

    EMaterialType mType = EMaterialType::Unlit;
    VkPipeline mPipeline = VK_NULL_HANDLE;
    glm::vec4 mColor = glm::vec4(1.0f);
    SDL_GPUTexture* mTexture = nullptr;
};

} // namespace Material