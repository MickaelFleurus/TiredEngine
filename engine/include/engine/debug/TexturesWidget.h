#pragma once

#include <unordered_map>

#include <imgui.h>

#include "engine/debug/IOverlordItem.h"

namespace Renderer {
class CTextureManager;
} // namespace Renderer

namespace Debug {

class CTexturesWidget : public IOverlordItem {
public:
    CTexturesWidget(Renderer::CTextureManager& textureManager);
    ~CTexturesWidget() override;

    const char* GetName() const override;
    void Render() override;

private:
    Renderer::CTextureManager& mTextureManager;
    std::unordered_map<int, ImTextureID> mImGuiTextureIDs;
};

} // namespace Debug
