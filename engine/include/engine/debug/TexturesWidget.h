#pragma once

#include <unordered_map>

#include <imgui.h>

#include "engine/debug/IOverlordItem.h"

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace Debug {
class CTextureImGuiContainer;
class CTexturesWidget : public IOverlordItem {
public:
    CTexturesWidget(CTextureImGuiContainer& textureContainer);

    const char* GetName() const override;
    void Render() override;

private:
    CTextureImGuiContainer& mTextureContainer;
};

} // namespace Debug
