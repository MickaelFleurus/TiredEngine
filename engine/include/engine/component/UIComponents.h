#pragma once
#include <cstddef>
#include <string>

#include <glm/vec4.hpp>

#include "engine/component/ComponentManager2.h"
#include "engine/core/DataTypes.h"
#include "engine/core/Entity.h"
#include "engine/utils/ComponentParserRegistry.h"
#include "engine/utils/ComponentUtils.h"
#include "engine/utils/Logger.h"

namespace Component {
struct SText {
    glm::vec2 anchor{0.0f};
    std::string content;
    std::string fontName;
    float fontSize{16.0f};
    glm::vec4 color{1.0f};
    bool isAnimated{false};

    std::vector<Core::SScreenQuadInstance>
        cachedGlyphs; // populated by build pass
    size_t lastContentHash{0};
    bool dirty{true};
};

struct SSprite {
    std::string spriteName;
    glm::vec4 color{1.0f};
    glm::vec2 size{};
    glm::vec2 anchor{0.0f};
    bool maintainAspectRatio{false};
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

static ComponentRegistrar RegisterSprite(
    "SpriteRenderer",
    [](Component::CManager& manager, Core::SEntity e, const nlohmann::json& j) {
        if (!j.contains("path")) {
            LOG_ERROR("The sprite component needs a path, and this object "
                      "does not have one.");
            return;
        }
        const auto spritePath = j.at("path").get<std::string>();
        auto& sprite = manager.Emplace<SSprite>(e);
        sprite.spriteName = spritePath;

        sprite.size =
            Utils::ParseArray2<float>(j, "size").value_or(glm::vec2{1.0f});

        sprite.maintainAspectRatio =
            Utils::ParseBool(j, "maintainAspectRatio").value_or(true);

        sprite.color =
            Utils::ParseArray4<float>(j, "color").value_or(glm::vec4{1.0f});
        sprite.anchor =
            Utils::ParseArray2<float>(j, "anchor").value_or(glm::vec2{1.0f});
    });

static ComponentRegistrar RegisterText("Text", [](Component::CManager& manager,
                                                  Core::SEntity e,
                                                  const nlohmann::json& j) {
    auto& text = manager.Emplace<SText>(e);
    text.content = j.at("value").get<std::string>();
    text.color =
        Utils::ParseArray4<float>(j, "color").value_or(glm::vec4{1.0f});
    text.fontSize = j.at("fontSize").get<float>();
    text.fontName = j.at("fontName").get<std::string>();

    text.isAnimated = Utils::ParseBool(j, "isAnimated").value_or(false);
});

} // namespace Component
