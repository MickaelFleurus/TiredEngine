#pragma once
#include <cstddef>
#include <string>

#include <glm/vec4.hpp>

#include "engine/component/ComponentManager2.h"
#include "engine/core/DataTypes.h"
#include "engine/core/Entity.h"
#include "engine/utils/ComponentParserRegistry.h"

namespace Component {
struct SText {
    std::string content;
    uint32_t fontId;
    float fontSize{16.0f};
    glm::vec4 color{1.0f};
    bool isAnimated{false};

    std::vector<Core::SScreenQuadInstance>
        cachedGlyphs; // populated by build pass
    size_t lastContentHash{0};
    bool dirty{true};
};

struct SSprite {
    uint32_t texIndex;
    glm::vec2 uvMin, uvMax;
    glm::vec4 color{1.0f};
    glm::vec2 size;
};

struct SScreenTransform {
    glm::vec2 anchor{0.0f};
    glm::vec2 position{0.0f};
    float rotation = 0.0f;
    glm::vec2 scale{1.0f};
    glm::mat3 resolvedTransform; // computed each frame
};

struct SHierarchy {
    CStringId parent{};
    std::vector<CStringId> children;
};

static ComponentRegistrar RegisterHierarchy(
    "Hierarchy",
    [](Component::CManager& manager, Core::SEntity e, const nlohmann::json& j) {
        auto& hierarchy = manager.Emplace<SHierarchy>(e);
        hierarchy.parent = CStringId(j.at("parent").get<std::string>());

        const auto names = j.at("children").get<std::vector<std::string>>();
        hierarchy.children = names |
                             std::views::transform([](const std::string& name) {
                                 return CStringId{name};
                             }) |
                             std::ranges::to<std::vector<CStringId>>();
    });

static ComponentRegistrar
    RegisterSprite("Sprite", [](Component::CManager& manager, Core::SEntity e,
                                const nlohmann::json& j) {
        auto& sprite = manager.Emplace<SSprite>(e);
        sprite.texIndex = j.at("texIndex").get<uint32_t>();
        const auto uv = j.at("uv").get<std::array<float, 4>>();
        sprite.uvMin = {uv[0], uv[1]};
        sprite.uvMax = {uv[2], uv[3]};
        sprite.color = {
            j.at("color").at(0).get<float>(), j.at("color").at(1).get<float>(),
            j.at("color").at(2).get<float>(), j.at("color").at(3).get<float>()};
        sprite.size = {j.at("size").at(0).get<float>(),
                       j.at("size").at(1).get<float>()};
    });

static ComponentRegistrar RegisterScreenTransform(
    "ScreenTransform",
    [](Component::CManager& manager, Core::SEntity e, const nlohmann::json& j) {
        auto& screenTransform = manager.Emplace<SScreenTransform>(e);
        screenTransform.anchor = {j.at("anchor").at(0).get<float>(),
                                  j.at("anchor").at(1).get<float>()};
        screenTransform.position = {j.at("position").at(0).get<float>(),
                                    j.at("position").at(1).get<float>()};
        screenTransform.rotation = j.at("rotation").get<float>();
        screenTransform.scale = {j.at("scale").at(0).get<float>(),
                                 j.at("scale").at(1).get<float>()};
    });

static ComponentRegistrar RegisterText("Text", [](Component::CManager& manager,
                                                  Core::SEntity e,
                                                  const nlohmann::json& j) {
    auto& text = manager.Emplace<SText>(e);
    text.content = j.at("value").get<std::string>();
    text.color = {
        j.at("color").at(0).get<float>(), j.at("color").at(1).get<float>(),
        j.at("color").at(2).get<float>(), j.at("color").at(3).get<float>()};
    text.fontSize = j.at("fontSize").get<float>();
    text.fontId = j.at("fontId").get<uint32_t>();
    if (j.contains("isAnimated")) {
        text.isAnimated = j.at("isAnimated").get<bool>();
    }
});

} // namespace Component
