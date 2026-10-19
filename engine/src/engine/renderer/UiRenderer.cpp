#include "engine/renderer/UiRenderer.h"

#include <filesystem>

#include "engine/component/ComponentManager2.h"
#include "engine/component/ComponentPool.h"
#include "engine/component/ComponentView.h"
#include "engine/component/TransformComponent.h"
#include "engine/component/UIComponents.h"
#include "engine/core/CameraUI.h"
#include "engine/core/EntityManager.h"
#include "engine/font/FontHandler.h"
#include "engine/font/Police.h"
#include "engine/renderer/PipelineTypes.h"
#include "engine/renderer/SpriteManager.h"
#include "engine/renderer/TextureManager.h"
#include "engine/system/System.h"
#include "engine/vulkan/HostBuffer.h"
#include "engine/vulkan/PipelineFactory.h"

#include "glm/ext/vector_float2.hpp"

namespace {

float twistAngleY(const glm::quat& q) {
    float len = std::sqrt(q.w * q.w + q.y * q.y);
    if (len < 1e-6f)
        return 0.0f;

    // angle = 2 * atan2(y, w) of the normalized twist quat
    float angle = 2.0f * std::atan2(q.y, q.w);

    // wrap to (-π, π]
    if (angle > glm::pi<float>())
        angle -= glm::two_pi<float>();
    if (angle <= -glm::pi<float>())
        angle += glm::two_pi<float>();
    return angle;
}

struct S2DTransform {
    S2DTransform(const Component::SLocalTransform& local)
        : position{local.position.x, local.position.y}
        , scale{local.scale.x, local.scale.y}
        , rotation(twistAngleY(local.rotation)) {
    }

    glm::vec2 position;
    glm::vec2 scale;
    float rotation;
};

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
            inst.isText = 1;

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

std::optional<Core::SScreenQuadInstance>
BuildSpriteQuad(const Component::SSprite& sprite, S2DTransform transform,
                const Renderer::CSpriteManager& spriteManager,
                const Renderer::CTextureManager& textureManager) {
    const auto spriteInfo = spriteManager.GetSpriteInfo(sprite.spriteName);
    if (!spriteInfo) {
        return std::nullopt;
    }

    const glm::vec2 naturalSize = spriteInfo->pixelSize;
    glm::vec2 size = naturalSize;
    const bool hasWidthOverride = sprite.size.x > 0.0f;
    const bool hasHeightOverride = sprite.size.y > 0.0f;
    if (sprite.maintainAspectRatio) {
        if (hasWidthOverride && hasHeightOverride) {
            const float fitScale = std::min(sprite.size.x / naturalSize.x,
                                            sprite.size.y / naturalSize.y);
            size = naturalSize * fitScale;
        } else if (hasWidthOverride) {
            size = naturalSize * (sprite.size.x / naturalSize.x);
        } else if (hasHeightOverride) {
            size = naturalSize * (sprite.size.y / naturalSize.y);
        }
    } else {
        if (hasWidthOverride) {
            size.x = sprite.size.x;
        }
        if (hasHeightOverride) {
            size.y = sprite.size.y;
        }
    }
    size *= transform.scale;

    Core::SScreenQuadInstance inst{};
    inst.posMin = transform.position;
    inst.posMax = transform.position + size;
    inst.uvMin = spriteInfo->uvMin;
    inst.uvMax = spriteInfo->uvMax;
    inst.color = sprite.color;
    inst.anchor = sprite.anchor;
    inst.scale = {1.0f, 1.0f};
    inst.rotation = transform.rotation;
    inst.texIndex = static_cast<uint32_t>(spriteInfo->id);
    inst.isText = 0u;
    return inst;
}

} // namespace

namespace Renderer {
CUiRenderer::CUiRenderer(
    const Vulkan::SContext& context, Component::CManager& componentManager,
    Font::CFontHandler& fontHandler, Renderer::CTextureManager& textureManager,
    Renderer::CSpriteManager& spriteManager, System::CSystem& system,
    Vulkan::CHostBuffer& instanceBuffer,
    Vulkan::CPipelineFactory& pipelineFactory,
    Core::CEntityManager& entityManager, Core::CCameraUI& uiCamera)
    : mContext(context)
    , mComponentManager(componentManager)
    , mFontHandler(fontHandler)
    , mTextureManager(textureManager)
    , mSpriteManager(spriteManager)
    , mPushConstants{.screenSize = {static_cast<float>(
                                        system.GetDisplayParameters().width),
                                    static_cast<float>(
                                        system.GetDisplayParameters().height)}}
    , mInstanceBuffer(instanceBuffer)
    , mEntityManager(entityManager)
    , mUiCamera(uiCamera) {
    Renderer::SPipelineConfig uiConfig{};
    uiConfig.shaderName = "UIShader";
    uiConfig.shaderPath = system.GetFileHandler().GetAssetsFolder("shaders");
    uiConfig.cullMode = Renderer::ECullMode::None;
    uiConfig.enableBlending = true;
    uiConfig.enableDepthTest = false;
    uiConfig.pushConstantSize = sizeof(Core::PC::SUiPushConstants);
    mUiPipelineDescriptors =
        pipelineFactory.GetOrCreateGraphicsPipeline(uiConfig);
}

void CUiRenderer::Prepare() {
    mInstances.clear();
    Component::CView textView{
        mComponentManager
            .GetPools<Component::SLocalTransform, Component::SText>()};
    for (auto it = textView.begin(); it != textView.end(); ++it) {
        auto& text = it.Get<Component::SText>();
        auto& transform = it.Get<Component::SLocalTransform>();
        it.Update<Component::SText>([&](auto& t) {
            t.cachedGlyphs = BuildScreenText(
                text.content, transform.position, text.fontSize, text.color,
                mFontHandler.GetPolice(text.fontName));
            t.dirty = false;
        });
        mInstances.insert(mInstances.end(), text.cachedGlyphs.begin(),
                          text.cachedGlyphs.end());
    }

    Component::CView spriteView{
        mComponentManager
            .GetPools<Component::SLocalTransform, Component::SSprite>()};
    for (auto it = spriteView.begin(); it != spriteView.end(); ++it) {
        auto& sprite = it.Get<Component::SSprite>();
        auto& xform = it.Get<Component::SLocalTransform>();
        const auto instance =
            BuildSpriteQuad(sprite, xform, mSpriteManager, mTextureManager);
        if (instance) {
            mInstances.push_back(*instance);
        }
    }
}

void CUiRenderer::Update() {
    mInstanceBuffer.ReplaceData(mInstances);
}

void CUiRenderer::Render(VkCommandBuffer cmd) {
    mPushConstants.instances = mInstanceBuffer.GetDeviceAddress();
    mPushConstants.screenSize = mUiCamera.GetCanvasSize();
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                      mUiPipelineDescriptors.pipeline);
    vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            mUiPipelineDescriptors.pipelineLayout, 0, 1,
                            &mContext.bindelessTextureSet, 0, nullptr);

    vkCmdPushConstants(cmd, mUiPipelineDescriptors.pipelineLayout,
                       VK_SHADER_STAGE_VERTEX_BIT |
                           VK_SHADER_STAGE_FRAGMENT_BIT,
                       0, sizeof(mPushConstants), &mPushConstants);
    vkCmdDraw(cmd, 6, mInstances.size(), 0, 0);
    mInstances.clear();
}
} // namespace Renderer
