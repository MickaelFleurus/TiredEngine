#include "engine/renderer/TextRenderer.h"

#include "engine/component/ComponentManager2.h"
#include "engine/component/ComponentView.h"
#include "engine/component/TransformComponent.h"
#include "engine/component/UIComponent.h"

namespace Renderer {
CTextRenderer::CTextRenderer(Component::CManager& componentManager,
                             Font::CFontHandler& fontHandler)
    : mComponentManager(componentManager), mFontHandler(fontHandler) {
}

void CTextRenderer::Prepare() {
    Component::CView view{
        mComponentManager
            .GetPools<Component::SWorldTransform, Component::SText>()};
    for (const auto& it : view) {
    }
    // const auto& metrics = mPolice->GetFontMetrics();
    // float scale = mFontSize / metrics.emSize;

    // float baselineY = metrics.ascenderY * scale;
    // float cursorX = 0.0f;
    // float cursorY = 0.0f;

    // for (char c : mText) {
    //     const Font::GlyphInfo& glyph = mPolice->GetGlyphInfo(c);

    //     if (c == '\n') {
    //         cursorX = 0.0f;
    //         cursorY += scale * metrics.lineHeight;
    //         continue;
    //     }

    //     if (c == ' ') {
    //         cursorX += glyph.advance * scale;
    //         continue;
    //     }

    //     glm::vec4 uvRect(glyph.uv.x, glyph.uv.y, glyph.uv.z, glyph.uv.w);

    //     Core::SUIInstanceData instance;
    //     instance.textureId = mPolice->GetTextureIndex();
    //     instance.materialId = 0;
    //     instance.color = mColor;

    //     glm::vec3 glyphPos(cursorX + glyph.offset.x * scale,
    //                        cursorY + baselineY - glyph.offset.y * scale -
    //                            glyph.size.y * scale,
    //                        0.0f);
    //     instance.modelMatrix = glm::translate(glm::mat4{1.0f}, glyphPos);
    //     instance.modelMatrix = glm::scale(
    //         instance.modelMatrix,
    //         glm::vec3(glyph.size.x * scale, glyph.size.y * scale, 1.0f));
    //     instance.modelMatrix = entityModel * instance.modelMatrix;
    //     instance.uvRect = uvRect;
    //     mInstances.push_back(instance);

    //     cursorX += glyph.advance * scale;
    // }
}
} // namespace Renderer
