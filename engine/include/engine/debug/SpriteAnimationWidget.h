#pragma once

#include <vulkan/vulkan.h>

#include "engine/debug/IOverlordItem.h"

namespace Renderer {
class CSpriteManager;
class CTextureManager;
} // namespace Renderer

struct SDL_Window;

namespace Debug {
class CSpriteAnimationWidget : public IOverlordItem {
public:
    CSpriteAnimationWidget(Renderer::CSpriteManager& spriteManager,
                           Renderer::CTextureManager& textureManager,
                           SDL_Window* window);
    ~CSpriteAnimationWidget() override;

    void Render() override;
    const char* GetName() const override;

private:
    Renderer::CSpriteManager& mSpriteManager;
    Renderer::CTextureManager& mTextureManager;
    SDL_Window* mWindow;
};
} // namespace Debug
