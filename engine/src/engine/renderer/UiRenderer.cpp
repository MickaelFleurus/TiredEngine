#include "engine/renderer/UiRenderer.h"

#include "engine/component/ComponentManager2.h"
#include "engine/component/ComponentPool.h"
#include "engine/component/ComponentView.h"
#include "engine/component/UIComponents.h"
#include "engine/font/FontHandler.h"
#include "engine/font/Police.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/system/System.h"
#include "engine/vulkan/HostBuffer.h"
#include "engine/vulkan/PipelineFactory.h"

namespace {

std::vector<Core::SScreenQuadInstance>
BuildScreenText(const std::string& utf8Text, glm::vec2 penStart, float fontSize,
                glm::vec4 color, const Font::CPolice& police) {
    const auto metrics = police.GetFontMetrics();

    std::vector<Core::SScreenQuadInstance> out;
    out.reserve(utf8Text.size());

    glm::vec2 pen = penStart;

    for (unsigned char c : utf8Text) { // ASCII path; see note below for UTF-8
        if (c == '\n') {
            pen.x = penStart.x;
            pen.y += static_cast<float>(metrics.lineHeight) * fontSize;
            continue;
        }

        const auto glyph = police.GetGlyphInfo(c);
        if (!glyph)
            continue;

        if (c != ' ') {
            Core::SScreenQuadInstance inst{};
            // y-up em space -> y-down screen space: top(t) is above baseline ==
            // smaller screen Y
            inst.posMin = {pen.x + glyph->planeBounds.x * fontSize,
                           pen.y - glyph->planeBounds.w * fontSize};
            inst.posMax = {pen.x + glyph->planeBounds.z * fontSize,
                           pen.y - glyph->planeBounds.y * fontSize};

            // atlas pixels -> normalized UV; flip V if your atlas rows are y-up
            // on upload
            inst.uvMin = glyph->uvMin;
            inst.uvMax = glyph->uvMax;

            inst.color = color;
            inst.texIndex = police.GetTextureIndex();

            out.push_back(inst);
        }

        double advance = glyph->advance;
        pen.x += static_cast<float>(advance) * fontSize;
    }

    return out;
}

glm::mat3 MakeLocal2D(glm::vec2 position, float rotation, glm::vec2 scale) {
    glm::mat3 t = glm::mat3(1.0f);
    t[2] = glm::vec3(position, 1.0f);

    float c = cos(rotation), s = sin(rotation);
    glm::mat3 r(c, s, 0, -s, c, 0, 0, 0, 1);

    glm::mat3 sc(scale.x, 0, 0, 0, scale.y, 0, 0, 0, 1);

    return t * r * sc; // scale first, then rotate, then translate
}

void ResolveScreenTransform(
    Component::CPool<Component::SScreenTransform>& screenTransformPool,
    Component::CPool<Component::SHierarchy>& hierarchyPool, Core::SEntity e,
    glm::mat3 parentTransform = glm::mat3{1.0f},
    glm::vec2 parentSize = glm::vec2(1.0f)) {
    screenTransformPool.Update(e, [&](auto& t) {
        glm::vec2 anchorOffset = t.anchor * parentSize;
        glm::mat3 local =
            MakeLocal2D(anchorOffset + t.position, t.rotation, t.scale);

        t.resolvedTransform = parentTransform * local;
    });

    if (hierarchyPool.Has(e)) {
        const auto& hierarchy = hierarchyPool.Get(e);
        const auto& parent = screenTransformPool.Get(e);
        for (Core::SEntity child : hierarchy.children) {
            ResolveScreenTransform(screenTransformPool, hierarchyPool, child,
                                   parent.resolvedTransform);
        }
    }
}
} // namespace

namespace Renderer {
CUiRenderer::CUiRenderer(Component::CManager& componentManager,
                         Font::CFontHandler& fontHandler,
                         System::CSystem& system,
                         Vulkan::CHostBuffer& instanceBuffer,
                         Vulkan::CPipelineFactory& pipelineFactory)
    : mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mPushConstants{.screenSize = {static_cast<float>(
                                        system.GetDisplayParameters().width),
                                    static_cast<float>(
                                        system.GetDisplayParameters().height)}}
    , mInstanceBuffer(instanceBuffer) {
    Renderer::SComputePipelineConfig uiConfig{};
    uiConfig.shaderPath = "shaders/ui.comp.spv";
    uiConfig.shaderName = "ui.comp.spv";
    mUiPipelineDescriptors =
        pipelineFactory.GetOrCreateComputePipeline(uiConfig);
}

void CUiRenderer::Prepare() {
    Component::CView textView{
        mComponentManager
            .GetPools<Component::SScreenTransform, Component::SText>()};
    for (auto it = textView.begin(); it != textView.end(); ++it) {
        auto& text = it.Get<Component::SText>();
        auto& transform = it.Get<Component::SScreenTransform>();
        if (text.dirty || text.isAnimated) {
            it.Update<Component::SText>([&](auto& t) {
                t.cachedGlyphs = BuildScreenText(
                    text.content, transform.position, text.fontSize, text.color,
                    *mFontHandler.GetPolice(text.fontId));
                t.dirty = false;
            });
        }
        mInstances.insert(mInstances.end(), text.cachedGlyphs.begin(),
                          text.cachedGlyphs.end());
    }

    Component::CView spriteView{
        mComponentManager
            .GetPools<Component::SScreenTransform, Component::SSprite>()};
    for (auto it = spriteView.begin(); it != spriteView.end(); ++it) {
    }
    //     auto& sprite = spriteView.get<CSpriteComponent>(e);
    //     auto& xform = spriteView.get<CScreenTransform>(e);
    //     screenQuadInstances.push_back(
    //         BuildSpriteQuad(sprite, xform.resolvedTransform));

    //
}

void CUiRenderer::Update() {
    instanceBuffer.ReplaceData(mInstances);
}

void CUiRenderer::Render(VkCommandBuffer cmd, VkPipeline uiPipeline,
                         Vulkan::CHostBuffer& instanceBuffer,
                         VkPipelineLayout uiPipelineLayout) {
    mPushConstants.instances = instanceBuffer.GetDeviceAddress();
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, uiPipeline);
    vkCmdPushConstants(cmd, uiPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(mPushConstants), &mPushConstants);
    vkCmdDraw(cmd, 6, mInstances.size(), 0, 0);
    mInstances.clear();
}
} // namespace Renderer
