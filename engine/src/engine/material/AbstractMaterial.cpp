#include "engine/material/AbstractMaterial.h"

namespace Material {
AbstractMaterial::AbstractMaterial(EMaterialType type, VkPipeline pipeline)
    : mType(type), mPipeline(pipeline) {
}

AbstractMaterial::AbstractMaterial(const AbstractMaterial& other)
    : mType(other.mType)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTexture(other.mTexture) {
}

AbstractMaterial& AbstractMaterial::operator=(const AbstractMaterial& other) {
    if (this != &other) {
        mType = other.mType;
        mPipeline = other.mPipeline;
        mColor = other.mColor;
        mTexture = other.mTexture;
    }
    return *this;
}

AbstractMaterial::AbstractMaterial(AbstractMaterial&& other) noexcept
    : mType(other.mType)
    , mPipeline(other.mPipeline)
    , mColor(other.mColor)
    , mTexture(other.mTexture) {
    other.mPipeline = nullptr;
    other.mTexture = nullptr;
}

AbstractMaterial&
AbstractMaterial::operator=(AbstractMaterial&& other) noexcept {
    if (this != &other) {
        mType = other.mType;
        mPipeline = other.mPipeline;
        mColor = other.mColor;
        mTexture = other.mTexture;
        other.mPipeline = nullptr;
        other.mTexture = nullptr;
    }
    return *this;
}

EMaterialType AbstractMaterial::GetType() const {
    return mType;
}

VkPipeline AbstractMaterial::GetPipeline() const {
    return mPipeline;
}

void AbstractMaterial::SetColor(const glm::vec4& color) {
    mColor = color;
}

void AbstractMaterial::SetTexture(SDL_GPUTexture* texture) {
    mTexture = texture;
}

const glm::vec4& AbstractMaterial::GetColor() const {
    return mColor;
}

SDL_GPUTexture* AbstractMaterial::GetTexture() const {
    return mTexture;
}

void AbstractMaterial::Bind(VkRenderPass renderPass) {
    // Default implementation does nothing
}

} // namespace Material