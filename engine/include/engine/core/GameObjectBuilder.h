#pragma once

#include "engine/core/GameObject.h"
#include "engine/input/InputCallback.h"
// #include "engine/physics/CollisionInfo.h"
#include "engine/utils/Anchors.h"

#include <functional>
#include <optional>
#include <string>

namespace Component {
class CComponentManager;
}

namespace Scene {
class CAbstractScene;
}

namespace Font {
class CFontHandler;
}

namespace Core {
class CGameObjectBuilder {
public:
    struct CInputCallbacks {
        std::optional<Input::InputFunc> onSpacePressed{std::nullopt};
        std::optional<Input::InputFunc> onLeftPressed{std::nullopt};
        std::optional<Input::InputFunc> onRightPressed{std::nullopt};
    };

    class CBuilder {
    public:
        CBuilder(const std::string& name,
                 Component::CComponentManager& componentManager,
                 Font::CFontHandler& fontHandler, CGameObject& parent);

        CBuilder& addText(const std::string& text, unsigned int size,
                          std::string fontName = "Arial");
        CBuilder& addSprite();
        CBuilder& addAABBCollisionData(float width, float height,
                                       bool isStatic = true,
                                       bool isTrigger = false);
        CBuilder& addCircleCollisionData(float radius, bool isStatic = true,
                                         bool isTrigger = false);
        CBuilder& addCameraComponent();
        // CBuilder& addCollisionCallback(
        //     std::function<void(Physics::SCollisionInfo)> callback);
        CBuilder& addMovementData(float acceleration);
        CBuilder& addInputInfo(CInputCallbacks callbacks);
        CBuilder& setLocalPosition(const glm::vec3& position);
        CBuilder& setAnchor(Utils::EAnchors anchor);
        CGameObject* build();

    private:
        Component::CComponentManager& mComponentManager;
        Font::CFontHandler& mFontHandler;
        CGameObject& mParent;
        std::unique_ptr<CGameObject> mGameObject;
    };

    explicit CGameObjectBuilder(Component::CComponentManager& componentManager,
                                Font::CFontHandler& fontHandler);
    CBuilder CreateBuilder(const std::string& name, CGameObject& parent);

private:
    static CGameObject*
    createRoot(Component::CComponentManager& componentManager);

    static GameObjectId mNextId;
    Component::CComponentManager& mComponentManager;
    Font::CFontHandler& mFontHandler;

    friend class Scene::CAbstractScene;
};

} // namespace Core